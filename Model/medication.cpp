#include "medication.h"

Medication::Medication(QSqlDatabase* database, unsigned int dbId)
    :dbId(dbId), counts { &countMo, &countTu, &countWe, &countTh, &countFr, &countSa, &countSu }
{
    name.init(database, dbId);
    countMo.init(database, dbId);
    countTu.init(database, dbId);
    countWe.init(database, dbId);
    countTh.init(database, dbId);
    countFr.init(database, dbId);
    countSa.init(database, dbId);
    countSu.init(database, dbId);
    name.Get();//ensure dbId is valid
}

Medication::Medication(const Medication &source)
    :dbId(source.dbId), counts { &countMo, &countTu, &countWe, &countTh, &countFr, &countSa, &countSu }
{
    name = source.name;
    countMo = source.countMo;
    countTu = source.countTu;
    countWe = source.countWe;
    countTh = source.countTh;
    countFr = source.countFr;
    countSa = source.countSa;
    countSu = source.countSu;
}

Medication::~Medication()
{

}

unsigned int Medication::GetDbId() const
{
    return dbId;
}

QString Medication::GetName() {
    return name.Get();
}

Medication Medication::CreateNew(QSqlDatabase *database, QString name)
{
    QSqlQuery query(*database);
    query.prepare("INSERT INTO Medication (name) VALUES (:name)");
    query.bindValue(":name", name);
    if(!query.exec())
        throw std::runtime_error("DB-Fehler: Kann Medikament nicht anlegen!");
    return Medication(database, query.lastInsertId().toUInt());
}

void Medication::CreateSchema(QSqlDatabase *database)
{
    QSqlQuery query(*database);
    if (!query.exec("CREATE TABLE IF NOT EXISTS medication(medicationId INTEGER PRIMARY KEY, name TEXT NOT NULL, "
                    "countMo INTEGER DEFAULT 0, countTu INTEGER DEFAULT 0, countWe INTEGER DEFAULT 0, "
                    "countTh INTEGER DEFAULT 0, countFr INTEGER DEFAULT 0, countSa INTEGER DEFAULT 0, "
                    "countSu INTEGER DEFAULT 0);"))
        throw std::runtime_error("Cannot create database schema");
}
