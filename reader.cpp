#include "reader.h"
#include <iostream>
#include <string>

Reader::Reader(QObject *parent) : QObject(parent), serialConn(this),
    readerState(NONE), readBuffer(), pollTimer(this)
{
    serialConn.close();
    serialConn.setPortName("/dev/ttyUSB0");

    if(!serialConn.open(QSerialPort::ReadWrite))
        throw std::runtime_error("Cannot open Serial port");

    serialConn.setBaudRate(9600);
    serialConn.setDataBits(QSerialPort::DataBits::Data8);
    serialConn.setStopBits(QSerialPort::StopBits::OneStop);
    serialConn.setParity(QSerialPort::Parity::NoParity);
    serialConn.setFlowControl(QSerialPort::FlowControl::NoFlowControl);

    connect(&serialConn, SIGNAL(readyRead()), this, SLOT(readSerial()));

    doRFC();

    pollTimer.setSingleShot(false);
    connect(&pollTimer, SIGNAL(timeout()), this, SLOT(startPoll()));
}

void Reader::readSerial()
{
    auto data = serialConn.readAll();
    for(auto datait = data.begin(); datait != data.end(); ++datait) {
        //std::cout << (int)(*datait)<<std::endl;
        if(*datait == '\r')
            processBuffer();
        else
            readBuffer.append(*datait);
    }

    //std::cout << std::string(data.constData(), data.length()) << std::endl;
}

void Reader::startPoll()
{
    if(readerState != STATE::IDLE)
        return; //Etwas anderes nutzt die Schnitstelle gerade!
    doTIF();
}

void Reader::processBuffer()
{
    switch(readerState) {
        case STATE::NONE:
            break; //Connected etc ignorieren
        case STATE::RFC:
            checkOK(std::runtime_error("Cannot set RF-Field to on"));
            doOEC();
            break;
        case STATE::OEC:
            checkOK(std::runtime_error("Cannot set OutputEnable to on"));
            doATC();
            break;
        case STATE::ATC:
            checkOK(std::runtime_error("Cannot set AntennaTuning to slow"));
            doSRT();
            break;
        case STATE::SRT:
            checkOK(std::runtime_error("Cannot set ReaderTag to 5567"));
            doSRM();
            break;
        case STATE::SRM:
            checkOK(std::runtime_error("Cannot set ReaderModulation to MAN"));
            std::cout << "Reader initialized" << std::endl;
            ConnectionEstablished();
            readerState = STATE::IDLE;
            pollTimer.start(POLLTIME);
            break;
        case STATE::IDLE:
            //
            break;
        case STATE::POLL_TIF:
            checkOK(std::runtime_error("Cannot test if tag present"));
            processTIF();
            break;
        case STATE::RSS:
            processRSS(); //Errorhandling in processRSS
            break;
    }
    readBuffer.clear();
}

void Reader::checkOK(const std::exception& ex)
{
    if(memcmp(readBuffer.constData(), "OK", 2) != 0)
    {
        std::cerr << "Reader returned error:"
                  << std::string(readBuffer.constData(), readBuffer.length()) << std::endl;
        throw ex;
    }
}

void Reader::doRFC()
{
    readerState = STATE::RFC;
    serialConn.write("CMRFC0016ON\r");
    serialConn.flush();
}

void Reader::doOEC()
{
    readerState = STATE::OEC;
    serialConn.write("CMOEC0016ON\r");
    serialConn.flush();
}

void Reader::doATC()
{
    readerState = STATE::ATC;
    serialConn.write("CMATC0032SLOW\r");
    serialConn.flush();}

void Reader::doSRT()
{
    readerState = STATE::SRT;
    serialConn.write("CMSRT00325567\r");
    serialConn.flush();
}

void Reader::doSRM()
{
    readerState = STATE::SRM;
    serialConn.write("RDSRM0024MAN\r");
    serialConn.flush();
}

void Reader::doTIF()
{
    readerState = STATE::POLL_TIF;
    serialConn.write("CMTIF0000\r"); // Sende Tag in Field
    serialConn.flush();
}

void Reader::processTIF()
{
    if(memcmp(readBuffer.constData(), "OK0007", 6) == 0)
    {//Falls kein Tag im Feld ist, wieder zu IDLE wechseln
        readerState = STATE::IDLE;
        return;
    }
    //Ansonsten Tag lesen
    doRSS();
}

void Reader::doRSS()
{
    readerState = STATE::RSS;
    serialConn.write("RDRSS0224\r"); //Lese 32bit (1.Block) per TerminatorSync
    serialConn.flush();
}

void Reader::processRSS() {
    if(memcmp(readBuffer.constData(), "OK0057 ",7) != 0) {
        std::cerr << "Reader returned error:"
                  << std::string(readBuffer.constData(), readBuffer.length()) << std::endl;
        throw std::runtime_error("Cannot read Tag Data: unexpected response");
    }
    std::string tagId = std::string(readBuffer.constData() + 7, 8);
    std::cout << "Tag: " << tagId << std::endl;
    readerState = STATE::IDLE;
}
