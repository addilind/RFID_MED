#ifndef UNIT_H
#define UNIT_H

#include <QSqlDatabase>
#include "dbprop.h"
#include "medication.h"

class Unit //Einzelne Schachtel, jeweils mit eigenem Tag
{
public:
    Unit(QSqlDatabase* database, unsigned int tagId);
    Unit(const Unit& source);

    static void CreateSchema(QSqlDatabase* database);
    unsigned int GetTagId() const;
    bool IsMedicationSet();
    void SetMedication(const Medication& med);
    Medication GetMedication();

    void Seen();
    unsigned int GetTimesSeen();
private:
    unsigned int tagId;
    unsigned int rowId;
    QSqlDatabase* db;

    //Strings für compile-time dbprops anlegen
    constStr(tableName, "knownTag") //SQL-Tabelle
    constStr(idCol, "rowid")        //Spalte, in der der Tabellen-Primärschlüssel steht, hier die automatisch vorhandene rowid
    constStr(medCol, "medicationId")//Spalte für Medikament-ID
    constStr(seenCol, "timesSeen")//Spalte für Zähler
    dbprop<unsigned int, tableName, medCol, idCol> medId;
    dbprop<unsigned int, tableName, seenCol, idCol> timesSeen;
};

#endif // UNIT_H
