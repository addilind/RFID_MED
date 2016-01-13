#include "medication.h"
#include "unit.h"

Medication::Medication(QSqlDatabase* database, unsigned int dbId)
    :dbId(dbId), db(database), counts { &countMo, &countTu, &countWe, &countTh, &countFr, &countSa, &countSu }
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

uint8_t Medication::GetDailyCount(uint day)
{
    if(day >= 7)
        throw std::runtime_error(qPrintable(QApplication::tr("Dosierung für ungültigen Tag abgefragt!")));
    return counts[day]->Get();
}

void Medication::SetName(const QString &newname)
{
    name.Set(newname);
}

void Medication::SetDailyCount(uint day, uint8_t count)
{
    if(day >= 7)
        throw std::runtime_error(qPrintable(QApplication::tr("Dosierung für ungültigen Tag gesetzt!")));
    return counts[day]->Set(count);
}

void Medication::Delete()
{
    QSqlQuery query(*db);
    query.prepare("DELETE FROM medication WHERE medicationId = :medId");
    query.bindValue(":medId", dbId);
    if(!query.exec())
        throw std::runtime_error("DB-Fehler: Kann Medikament nicht löschen!");
}

bool Medication::AnyUnitSeenToday()
{
    auto allUnits = Unit::GetByMedication(db, *this);
    return std::any_of( allUnits.begin(), allUnits.end(), [](Unit& i)
                   { return i.GetLastSeen().date() == QDate::currentDate(); });
}

Medication Medication::CreateNew(QSqlDatabase *database, const QString& name)
{
    QSqlQuery query(*database);
    query.prepare("INSERT INTO medication (name) VALUES (:name)");
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

unsigned int Medication::GetMedicationCount(QSqlDatabase *database)
{
    QSqlQuery query(*database);
    query.setForwardOnly(true);
    if (!query.exec("SELECT COUNT(medicationId) FROM medication;"))
        throw std::runtime_error("DB-Fehler: Kann Medikamente nicht zählen");
    if(!query.next())
        throw std::runtime_error("DB-Fehler: Kann Medikamente nicht zählen\nKein Ergebnis");
    return query.value(0).toUInt();
}

std::vector<uint> Medication::GetMedicationIds(QSqlDatabase *database)
{
    QSqlQuery query(*database);
    query.setForwardOnly(true);
    if (!query.exec("SELECT medicationId FROM medication ORDER BY name;"))
        throw std::runtime_error("DB-Fehler: Kann Medikamente nicht auflisten");
    std::vector<uint> result;
    while(query.next())
        result.push_back(query.value(0).toUInt());
    return result;
}
