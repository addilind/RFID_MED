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

    Medication GetMedication(uint id);
    Medication AddMedication(const QString& name);
    Unit GetUnit(uint tagId);
    uint GetMedicationCount();
    std::vector<uint> GetMedicationIds();
    std::vector<uint> GetUnitTagIds();
    std::vector<Unit> GetUnitsByMedication(const Medication& med);
signals:
    void UnitChanged(Unit* u);

public slots:
    void TagChanged(bool present, uint id);

private:
    QSqlDatabase db;

    void initializeDB();
};

#endif // DATASTORE_H
