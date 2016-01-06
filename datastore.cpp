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
    db.close();
}

void Datastore::initializeDB()
{
    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS medication(medicationId INTEGER, name TEXT);"))
        throw std::runtime_error("Cannot create database schema");
    if (!query.exec("CREATE TABLE IF NOT EXISTS knownTag(tagId INTEGER, medicationId INTEGER, "
                    "countMo INTEGER, countTu INTEGER, countWe INTEGER, countTh INTEGER, "
                    "countFr INTEGER, countSa INTEGER, countSu INTEGER);"))
        throw std::runtime_error("Cannot create database schema");

    db.commit();
}
