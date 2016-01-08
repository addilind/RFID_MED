#ifndef MEDICATION_H
#define MEDICATION_H

#include <QString>
#include "dbprop.h"

class Medication
{
public:
    Medication(QSqlDatabase* database, unsigned int dbId);
    Medication(const Medication& source);
    virtual ~Medication();

    unsigned int GetDbId() const;
    QString GetName();

    static Medication CreateNew(QSqlDatabase* database, QString name);
    static void CreateSchema(QSqlDatabase* database);

private:
    unsigned int dbId;

    //Strings für compile-time dbprops anlegen
    constStr(tableName, "medication") //SQL-Tabelle
    constStr(idCol, "medicationId")   //Spalte, in der der Schlüssel steht
    constStr(nameCol, "name")         //Spalte für name
    constStr(countMoCol, "countMo")
    constStr(countTuCol, "countTu")
    constStr(countWeCol, "countWe")
    constStr(countThCol, "countTh")
    constStr(countFrCol, "countFr")
    constStr(countSaCol, "countSa")
    constStr(countSuCol, "countSo")

    dbprop<QString, tableName, nameCol, idCol> name;
    dbprop<unsigned char, tableName, countMoCol, idCol> countMo;
    dbprop<unsigned char, tableName, countTuCol, idCol> countTu;
    dbprop<unsigned char, tableName, countWeCol, idCol> countWe;
    dbprop<unsigned char, tableName, countThCol, idCol> countTh;
    dbprop<unsigned char, tableName, countFrCol, idCol> countFr;
    dbprop<unsigned char, tableName, countSaCol, idCol> countSa;
    dbprop<unsigned char, tableName, countSuCol, idCol> countSu;
    dbpropg<unsigned char>* counts[7]; //Für leichteren Zugriff noch als array
};
//const char Medication::DBP_Medication[] = "Medication";

#endif // MEDICATION_H
