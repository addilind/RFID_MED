#include "reader.h"
#include <iostream>
#include <string>

Reader::Reader(const QString& portName, QObject *parent) : QObject(parent),
    readerState(NONE), serialConn(this), readBuffer(), pollTimer(this),
    tagPresent(false), tagId(0U)
{
    serialConn.close();
    serialConn.setPortName(portName);

    connect(&serialConn, SIGNAL(readyRead()), this, SLOT(readSerial()));

    pollTimer.setSingleShot(false);
    connect(&pollTimer, SIGNAL(timeout()), this, SLOT(startPoll()));
}

Reader::~Reader()
{
    serialConn.close();
}

void Reader::BeginConnect()
{
    if(!serialConn.open(QSerialPort::ReadWrite))
        throw std::runtime_error(qPrintable(tr("Serielle Schnittstelle '") +
                                            serialConn.portName() +
                                            tr("' konnte nicht geöffnet werden,\n"
                                               "bitte per Befehlszeilenoption -p festlegen!")));

    serialConn.setBaudRate(9600);
    serialConn.setDataBits(QSerialPort::DataBits::Data8);
    serialConn.setStopBits(QSerialPort::StopBits::OneStop);
    serialConn.setParity(QSerialPort::Parity::NoParity);
    serialConn.setFlowControl(QSerialPort::FlowControl::NoFlowControl);

    doRFC(); //beginne Initialisierung
}

bool Reader::getTagPresent()
{
    return tagPresent;
}

uint Reader::getLastTagId()
{
    return tagId;
}

void Reader::readSerial()
{//Wird aufgerufen, sobald Daten auf der seriellen Schnittstelle empfangen wurden
    auto data = serialConn.readAll();
    for(auto datait = data.begin(); datait != data.end(); ++datait) { //Zeichenweise gelesene Daten einlesen
        if(*datait == '\r') { //\r beendet eine Antwort, aktuellen Puffer verarbeiten
            try {
                processBuffer();
            }
            catch(std::exception& ex)
            {
                std::cerr << ex.what() << std::endl;
                ErrorOccured(ex);
            }
        }
        else //ansonsten an Puffer anhängen
            readBuffer.append(*datait);
    }
}

void Reader::startPoll()
{//Wird regelmäßig aufgerufen, um aktuelle Tags zu scannen
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
            checkOK("Kann RF-Feld nicht aktivieren");
            doOEC();
            break;
        case STATE::OEC:
            checkOK("Kann OutputEnable nicht aktivieren");
            doATC();
            break;
        case STATE::ATC:
            checkOK("Kann AntennaTuning nicht auf slow setzen");
            doSRT();
            break;
        case STATE::SRT:
            checkOK("Kann ReaderTag nicht auf 5567 setzen");
            doSRM();
            break;
        case STATE::SRM:
            checkOK("Kann ReaderModulation nicht auf MAN setzen");
            ConnectionEstablished();
            readerState = STATE::IDLE;
            pollTimer.start(POLLTIME);
            break;
        case STATE::IDLE:
            //This should never happen (TM)
            //Der Scanner hat etwas geantwortet, ohne das wir die Frage kennen
            //könnte bei Verlassen & erneutem Aufruf des seriellen Modus am Gerät auftreten,
            //aber das sollte man sowieso während des Betriebs nicht tun
            break;
        case STATE::POLL_TIF:
            checkOK("Kann nicht auf Vorhandensein von Tags prüfen");
            processTIF();
            break;
        case STATE::RSS:
            processRSS(); //Errorhandling übernimmt processRSS, da Länge auch überprüft wird
            break;
    }
    readBuffer.clear();
}

void Reader::checkOK(const char* ex_msg)
{//Überprüft, ob Antwort mit OK beginnt
    if(memcmp(readBuffer.constData(), "OK", 2) != 0)
    {
        auto msg = qPrintable(tr(ex_msg) + tr("\nAntwort: ") +
                              QString::fromLocal8Bit(readBuffer.constData(), readBuffer.length()));
        std::cerr << msg << std::endl;
        throw std::runtime_error(msg);
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

        if(tagPresent) //Falls beim letzten Scan ein Tag vorhanden war
        {
            tagPresent = false;
            TagChanged(false, tagId);//über Änderung benachrichtigen
        }
        return;
    }
    //Ansonsten Tag lesen
    doRSS();
}

void Reader::doRSS()
{
    readerState = STATE::RSS;
    serialConn.write("RDRSS0224\r"); //Lese 224bit via StreamSync
    serialConn.flush();
}

void Reader::processRSS() {
    if(readBuffer.size() < 15 || memcmp(readBuffer.constData(), "OK0057 ",7) != 0) {
        //Bei lesefehler von leerem Lesefeld ausgehen
        if(tagPresent)
            TagChanged(false, tagId);//über Änderung benachrichtigen

        tagPresent = false;
        readerState = STATE::IDLE;
        return;
    }
    const char* readPos = readBuffer.constData() + 7; //Überspringe OK & Länge

    QString tagId_str = QString::fromLatin1(readPos, 8);
    readPos += 8;
    bool parseSuccess = false;
    uint tagId = tagId_str.toInt(&parseSuccess, 16);
    if(!parseSuccess) {
        auto msg = qPrintable(tr("Kann Tag-ID nicht lesen (nicht Hexadezimal)\nTagID-Text: ") + tagId_str);
        std::cerr << msg << std::endl;
        throw std::runtime_error(msg);
    }

    bool isChange = !tagPresent; //Falls vorher kein Tag im Feld war ist es auf jeden Fall eine Änderung
    if(tagPresent) //Falls vorher ein Tag im Feld war auf gleiche Id überprüfen
        isChange = this->tagId != tagId;


    if(isChange) {
        tagPresent = true;
        this->tagId = tagId;
        TagChanged(true, tagId);//über Änderung benachrichtigen
    }

    readerState = STATE::IDLE;
}
