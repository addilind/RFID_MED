#ifndef MEDICATION_H
#define MEDICATION_H

#include <QString>
#include <vector>
#include "dbprop.h"

class Medication
{
public:
    Medication(QSqlDatabase* database, unsigned int dbId);
    Medication(const Medication& source);
    virtual ~Medication();

    uint GetDbId() const;
    QString GetName();
    uint8_t GetDailyCount(uint day);
    void SetName(const QString& name);
    void SetDailyCount(uint day, uint8_t count);
    void Delete();
    bool AnyUnitSeenToday();

    static Medication CreateNew(QSqlDatabase* database, const QString& name);
    static void CreateSchema(QSqlDatabase* database);
    static unsigned int GetMedicationCount(QSqlDatabase *database);
    static std::vector<uint> GetMedicationIds(QSqlDatabase *database);
private:
    unsigned int dbId;
    QSqlDatabase* db;

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
    constStr(countSuCol, "countSu")

    dbprop<QString, tableName, nameCol, idCol> name;
    dbprop<uint8_t, tableName, countMoCol, idCol> countMo;
    dbprop<uint8_t, tableName, countTuCol, idCol> countTu;
    dbprop<uint8_t, tableName, countWeCol, idCol> countWe;
    dbprop<uint8_t, tableName, countThCol, idCol> countTh;
    dbprop<uint8_t, tableName, countFrCol, idCol> countFr;
    dbprop<uint8_t, tableName, countSaCol, idCol> countSa;
    dbprop<uint8_t, tableName, countSuCol, idCol> countSu;
    dbpropg<uint8_t>* counts[7]; //Für leichteren Zugriff noch als array
};
//const char Medication::DBP_Medication[] = "Medication";

#endif // MEDICATION_H
