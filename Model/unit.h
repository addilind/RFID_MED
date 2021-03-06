#ifndef UNIT_H
#define UNIT_H

#include <stdint.h>
#include <QSqlDatabase>
#include <QDateTime>
#include "dbprop.h"
#include "medication.h"

class Unit //Einzelne Schachtel, jeweils mit eigenem Tag
{
public:
    Unit(QSqlDatabase* database, uint32_t tagId);
    Unit(const Unit& source);

    uint32_t GetTagId() const;
    bool IsMedicationSet();
    void SetMedication(const Medication& med);
    void UnsetMedication();
    Medication GetMedication();
    void Delete();

    void Seen();
    uint32_t GetTimesSeen();
    QDateTime GetLastSeen();

    static void CreateSchema(QSqlDatabase* database);
    static std::vector<uint32_t> GetTagIds(QSqlDatabase* database);
    static std::vector<Unit> GetByMedication(QSqlDatabase* database, const Medication& med);
private:
    unsigned int tagId;
    unsigned int rowId;
    QSqlDatabase* db;

    //Strings für compile-time dbprops anlegen
    constStr(tableName, "knownTag") //SQL-Tabelle
    constStr(idCol, "rowid")        //Spalte, in der der Tabellen-Primärschlüssel steht, hier die automatisch vorhandene rowid
    constStr(medCol, "medicationId")//Spalte für Medikament-ID
    constStr(seenCol, "timesSeen")  //Spalte für Zähler
    constStr(lastCol, "lastSeen")   //Spalte für zuletzt gesehen
    dbprop<uint32_t, tableName, medCol, idCol> medId;
    dbprop<uint32_t, tableName, seenCol, idCol> timesSeen;
    dbprop<uint32_t, tableName, lastCol, idCol> lastSeen;
};

#endif // UNIT_H
