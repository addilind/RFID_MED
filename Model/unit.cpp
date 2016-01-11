#include "unit.h"
#include <QApplication>
#include <QSqlError>
#include <QDebug>

Unit::Unit(QSqlDatabase *database, unsigned int tagId)
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
}

Unit::Unit(const Unit &source)
    : tagId(source.tagId), rowId(source.rowId), db(source.db), timesSeen(source.timesSeen)
{

}

void Unit::CreateSchema(QSqlDatabase *database)
{
    QSqlQuery query(*database);
    if (!query.exec("CREATE TABLE IF NOT EXISTS knownTag(tagId INTEGER UNIQUE, medicationId INTEGER, "
                    "timesSeen INTEGER DEFAULT 0, FOREIGN KEY(medicationId) REFERENCES medication(medicationId) );"))
        throw std::runtime_error(
                qPrintable(QApplication::tr("DB-Fehler:\nKann DatenbankSchema für Tags nicht erstellen:\n") + query.lastError().text()));
    query.finish();
    if (!query.exec("CREATE INDEX IF NOT EXISTS tagIdx ON knownTag(tagId);")) //Erstelle Index, um tagIds schnell nachschlagen zu können
        throw std::runtime_error(
                qPrintable(QApplication::tr("DB-Fehler:\nKann DatenbankIndex für TagIds nicht erstellen:\n") + query.lastError().text()));
}

unsigned int Unit::GetTagId() const
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

Medication Unit::GetMedication()
{
    return Medication(db, medId.Get());
}

void Unit::Seen()
{
    timesSeen.Set(timesSeen.Get() + 1);
}

unsigned int Unit::GetTimesSeen()
{
    return timesSeen.Get();
}
