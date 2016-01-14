#ifndef HOMESTATION_H
#define HOMESTATION_H


#define USESIM

#include <QMainWindow>
#include <QString>
#include "settings.h"
#include "datastore.h"
#include "reader.h"
#include "Model/medlist.h"

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

private slots:
    void openSettings();

private:
    Ui::Homestation *ui;
    Settings* settings;
    Datastore* dstore;
#ifdef USESIM
    Simulator* reader;
#else
    Reader* reader;
#endif
    QString serialPort;
    QString dbFileName;
    medList* meds;

    virtual void showEvent(QShowEvent *event) override;
    void unknownUnit();
    void showUnit(Unit* u);
};

#endif // HOMESTATION_H
