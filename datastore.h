#ifndef DATASTORE_H
#define DATASTORE_H

#include <QObject>
#include <QSqlDatabase>
#include "Model/medication.h"
#include "Model/unit.h"

class Datastore : public QObject
{
    Q_OBJECT
public:
    explicit Datastore(const QString& fileName, QObject *parent = 0);
    ~Datastore();

    Medication getMedication(uint id);
    Medication addMedication(QString& name);
    Unit getUnitInfo(uint tagId);
signals:
    void UnitChanged(Unit* u);

public slots:
    void TagChanged(bool present, uint id);

private:
    QSqlDatabase db;

    void initializeDB();
};

#endif // DATASTORE_H
