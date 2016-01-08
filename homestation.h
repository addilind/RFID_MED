#ifndef HOMESTATION_H
#define HOMESTATION_H


#define USESIM

#include <QMainWindow>
#include <QString>
#include "datastore.h"
#include "reader.h"

#ifdef USESIM
#include "simulator.h"
#endif

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
    void Ready();
    void SubmoduleError(const std::exception& ex);
    void UnitChanged(Unit* u);
private:
    Ui::Homestation *ui;
    Datastore* dstore;
#ifdef USESIM
    Simulator* reader;
#else
    Reader* reader;
#endif
    QString serialPort;
    QString dbFileName;

    virtual void showEvent(QShowEvent *event);
    void unknownUnit();
};

#endif // HOMESTATION_H
