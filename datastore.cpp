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

Medication Datastore::getMedication(uint id)
{
    return Medication(&db, id);
}

Medication Datastore::addMedication(QString& name)
{
    return Medication::CreateNew(&db, name);
}

Unit Datastore::getUnitInfo(uint tag)
{
    return Unit(&db, tag);
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
        emit UnitChanged(&u);
    }
    else
        emit UnitChanged(nullptr);
}
