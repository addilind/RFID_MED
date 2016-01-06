#ifndef READER_H
#define READER_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>

class Reader : public QObject
{
    Q_OBJECT
public:
    explicit Reader(const QString& portName, QObject *parent = 0);
    static const int POLLTIME = 2000;
    virtual ~Reader();
    void BeginConnect();

signals:
    void ConnectionEstablished();
    void ErrorOccured(const std::exception& exception_msg);
public slots:

private slots:
    void readSerial();
    void startPoll();

private:
    enum STATE { NONE, RFC, OEC, ATC, SRT, SRM, IDLE, POLL_TIF, RSS };
    STATE readerState;
    QSerialPort serialConn;
    QByteArray readBuffer;
    QTimer pollTimer;

    void processBuffer();

    void checkOK(const char* ex);
    void doRFC();
    void doOEC();
    void doATC();
    void doSRT();
    void doSRM();
    void doTIF();
    void processTIF();
    void doRSS();
    void processRSS();
};

#endif // READER_H
