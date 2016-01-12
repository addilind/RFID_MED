#include "datastore.h"
#include <QSqlQuery>

Datastore::Datastore(const QString& fileName, QObject *parent) : QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(fileName);
    if(!db.open())
        throw std::runtime_error(qPrintable(tr("Kann Datenbank '") + fileName + tr("' nicht öffnen")));
    initializeDB();
}

Datastore::~Datastore()
{
    db.commit();
    db.close();
}

Medication Datastore::GetMedication(uint id)
{
    return Medication(&db, id);
}

Medication Datastore::AddMedication(const QString& name)
{
    return Medication::CreateNew(&db, name);
}

Unit Datastore::GetUnit(uint tag)
{
    return Unit(&db, tag);
}

uint Datastore::GetMedicationCount()
{
    return Medication::GetMedicationCount(&db);
}

std::vector<uint> Datastore::GetMedicationIds()
{
    return Medication::GetMedicationIds(&db);
}

std::vector<uint> Datastore::GetUnitTagIds()
{
    return Unit::GetTagIds(&db);
}

std::vector<Unit> Datastore::GetUnitsByMedication(const Medication &med)
{
    return Unit::GetByMedication(&db, med);
}

void Datastore::initializeDB()
{
    QSqlQuery query(db);
    query.exec("PRAGMA foreign_keys = ON;");
    Medication::CreateSchema(&db);
    Unit::CreateSchema(&db);
    db.commit();
}

void Datastore::TagChanged(bool present, uint id)
{
    if(present) {
        Unit u(&db, id);
        u.Seen(); //Zähler erhöhen
        emit UnitChanged(&u);
    }
    else
        emit UnitChanged(nullptr);
}
