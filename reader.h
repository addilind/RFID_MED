#ifndef READER_H
#define READER_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>

class Reader : public QObject
{
    Q_OBJECT
public:
    explicit Reader(QObject *parent = 0);
    static const int POLLTIME = 2000;

signals:
    void ConnectionEstablished();
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

    void checkOK(const std::exception& ex);
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
