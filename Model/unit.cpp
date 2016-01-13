#include "unit.h"
#include <QApplication>
#include <QSqlError>
#include <QDebug>

Unit::Unit(QSqlDatabase *database, uint32_t tagId)
    : tagId(tagId), db(database)
{
    QSqlQuery query(*database);
    query.setForwardOnly(true);
    query.prepare("SELECT rowid FROM knownTag WHERE tagId = :tag LIMIT 1;");
    query.bindValue(":tag", tagId);
    if(!query.exec())
        throw std::runtime_error(
            qPrintable(QApplication::tr("DB-Fehler:\nKann Tag nicht auf Vorhandensein prüfen:\n") + query.lastError().text()));
    if(!query.next()) //Bisher unbekannter Tag
    {
        qInfo() << "Lege Tag an";
        query.prepare("INSERT INTO knownTag(tagId) VALUES (:tag);");
        query.bindValue(":tag", tagId);
        if(!query.exec())
            throw std::runtime_error(
                    qPrintable(QApplication::tr("DB-Fehler:\nKann Tag nicht hinzufügen:\n") + query.lastError().text()));
        rowId = query.lastInsertId().toUInt();
        db->commit();
    }
    else
    {
        rowId = query.value(0).toUInt();
    }
    medId.init(database, rowId);
    timesSeen.init(database, rowId);
    lastSeen.init(database, rowId);
}

Unit::Unit(const Unit &source)
    : tagId(source.tagId), rowId(source.rowId), db(source.db),
      medId(source.medId), timesSeen(source.timesSeen), lastSeen(source.lastSeen)
{

}

void Unit::CreateSchema(QSqlDatabase *database)
{
    QSqlQuery query(*database);
    if (!query.exec("CREATE TABLE IF NOT EXISTS knownTag(tagId INTEGER UNIQUE, medicationId INTEGER, "
                    "timesSeen INTEGER DEFAULT 0, lastSeen INTEGER DEFAULT (strftime('%s','now')), "
                    "FOREIGN KEY(medicationId) REFERENCES medication(medicationId) );"))
        throw std::runtime_error(
                qPrintable(QApplication::tr("DB-Fehler:\nKann DatenbankSchema für Tags nicht erstellen:\n") + query.lastError().text()));
    query.finish();
    if (!query.exec("CREATE INDEX IF NOT EXISTS tagIdx ON knownTag(tagId);")) //Erstelle Index, um tagIds schnell nachschlagen zu können
        throw std::runtime_error(
                qPrintable(QApplication::tr("DB-Fehler:\nKann DatenbankIndex für TagIds nicht erstellen:\n") + query.lastError().text()));
}

std::vector<uint32_t> Unit::GetTagIds(QSqlDatabase *database)
{
    QSqlQuery query(*database);
    query.setForwardOnly(true);
    if (!query.exec("SELECT tagId FROM knownTag ORDER BY tagId ASC;"))
        throw std::runtime_error("DB-Fehler: Kann Packungen nicht auflisten");
    std::vector<uint32_t> result;
    while(query.next())
        result.push_back(query.value(0).toUInt());
    return result;
}

std::vector<Unit> Unit::GetByMedication(QSqlDatabase* database, const Medication &med)
{
    QSqlQuery query(*database);
    query.setForwardOnly(true);
    query.prepare("SELECT tagId FROM knownTag WHERE medicationID = :medId;");
    query.bindValue(":medId", med.GetDbId());
    if (!query.exec())
        throw std::runtime_error("DB-Fehler: Kann nicht nach zu Medikament gehörenden Packungen suchen!");
    std::vector<Unit> result;
    while(query.next())
        result.push_back(Unit(database, query.value(0).toUInt()));
    return result;
}

uint32_t Unit::GetTagId() const
{
    return tagId;
}

bool Unit::IsMedicationSet()
{
    return !medId.IsNull(); //Medikament ist gesetzt
}

void Unit::SetMedication(const Medication &med)
{
    medId.Set(med.GetDbId());
}

void Unit::UnsetMedication()
{
    medId.Unset();
}

Medication Unit::GetMedication()
{
    return Medication(db, medId.Get());
}

void Unit::Delete()
{
    QSqlQuery query(*db);
    query.prepare("DELETE FROM knownTag WHERE rowid = :rowId");
    query.bindValue(":rowId", rowId);
    if(!query.exec())
        throw std::runtime_error("DB-Fehler: Kann Packung nicht löschen!");
}

void Unit::Seen()
{
    timesSeen.Set(timesSeen.Get() + 1);
    lastSeen.Set(QDateTime::currentDateTime().toTime_t());
}

QDateTime Unit::GetLastSeen()
{
    return QDateTime::fromTime_t(lastSeen.Get());
}

unsigned int Unit::GetTimesSeen()
{
    return timesSeen.Get();
}
