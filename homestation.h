#ifndef HOMESTATION_H
#define HOMESTATION_H

#include <QMainWindow>
#include <QString>
#include "datastore.h"
#include "reader.h"

namespace Ui {
class Homestation;
}

class Homestation : public QMainWindow
{
    Q_OBJECT

public:
    explicit Homestation(QWidget *parent = 0);
    ~Homestation();

    void SetSerialPort(const QString& portName);
    void SetDBFile(const QString& fileName);

public slots:
    void ReaderConnected();
    void SubmoduleError(const std::exception& ex);

private:
    Ui::Homestation *ui;
    Datastore* dstore;
    Reader* reader;
    QString serialPort;
    QString dbFileName;

    virtual void showEvent(QShowEvent *event);
};

#endif // HOMESTATION_H
