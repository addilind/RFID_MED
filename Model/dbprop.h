#ifndef DBPROP_H
#define DBPROP_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <assert.h>
#include <QString>
#include <QVariant>
#include <QApplication>

#include <QDebug>
#include <QSqlDriver>

#define DBP_TEMPPARA <T, table, valcol, idcol>
#define DBP_TEMPLATE template <typename T, typename table, typename valcol, typename idcol>

// ================================== Beginn schwarze Compilermagie ======================================
//compile-time string-concat, grob nach
// https://stackoverflow.com/questions/9050857/constexpr-with-string-operations-workaround/9054709#9054709
//
// Strings verketten funktioniert dabei nur, wenn sie als einelne chars über eine variable Templatedefinition
// (das ... im template; "variadic char parameter") angegeben werden (das übernehmen die oberen 3 Template-
// structs). Da die Nutzung als templateString<'S', 'E', 'L', ...> nicht benutzerfreundlich ist, gibt es noch
// die unteren Templates, die aus einem String-Literal einen templateString machen, indem sie iterativ ein
// Zeichen aus dem String aneinander concaten. Damit man ein Stringliteral als Templateparameter nutzen kann,
// benötigt es "external linkage", d.h. der String muss ausserhalb der Templateparameterliste definiert sein.
// Deshalb gibt es das constStr Makro, dass den gewünschten String als constexpr-Literal definiert, und danach
// das Converter-Template darauf nutzt. Da keine echten Variablen angelegt werden, sondern nur typedefs gefüllt
// und constexpr benannt werden, findet sich im resultierenden Programm nur noch der fertige SQL-Query-String

template <char ... characters>
struct templateString
{
    static QString string()
    {
        constexpr const char stringcont[] {characters...}; //Aus den einzelnen chars wieder ein char-Array machen
        //Zu beachten: alle 0-bytes wurden entfernt, deshalb muss die Überladung mit Längenangabe genutzt werden
        return QString::fromLatin1(stringcont, sizeof(stringcont));
    }
};

template <class L, class R>
struct templateConcat {};

template <char ... LC, char  ... RC>
struct templateConcat<templateString<LC ... >, templateString<RC ... >>
{
    typedef templateString<LC ..., RC ... > Result;
};

//String literal zu variadic char template conv
template<int len, const char str[len], int c>
struct constStrConv {//Zeichen anhängen, vorderen Teil rekursiv erzeugen
    typedef typename constStrConv<len, str, c-1>::Result prevIteration;
    typedef typename templateConcat<prevIteration, templateString<str[c]>>::Result Result;
};
template<int len, const char str[len]>
struct constStrConv<len, str, 0> { //Ende der Iteration, erstes Zeichen zurückggeben
    typedef templateString<str[0]> Result;
};
template<int len, const char str[len]>
struct constString {
    typedef typename constStrConv<len, str, len-2>::Result templateString;
    //-2, da letzter Index = - 1 & Null abschneiden
};

#define constStr(varname, content) static constexpr const char DBP_##varname [] = content; \
typedef typename constString<sizeof(content), DBP_##varname>::templateString varname;

// ================================== Ende schwarze Compilermagie ======================================

//Superklasse, die nur vom Typ abhängt, um Arrays von dbprops erzeugen zu können
template<typename T>
class dbpropg {
public:
    dbpropg();
    virtual ~dbpropg();
    virtual T Get() = 0;
    virtual void Set(const T& val) =0;
    virtual void Unset() = 0;
    virtual bool IsNull() = 0;
};

//DB-Property = eine Eigenschaft, deren Wert transparent direkt aus/in die Datenbank gelesen/geschrieben wird
//Dabei werden die SQL-Befehle über constexpressions und Templates schon zur compile-Zeit zusammengesetzt
//und die SQLQueries als PreparedStatements im Konstruktor erzeugt, um den Einfluss auf die Laufzeit gering zu halten
DBP_TEMPLATE
class dbprop : public dbpropg<T>
{
public:
    dbprop();
    dbprop(const dbprop DBP_TEMPPARA & source);
    virtual ~dbprop();
    void init(QSqlDatabase* database, uint pid);
    //Init ist nicht im Konstruktor, damit Nutzer DB-Initialisierung vorher durchführen können
    virtual T Get() override;
    virtual void Set(const T& val) override;
    virtual void Unset() override;
    virtual bool IsNull() override;

private:
    uint parentId;
    bool initialized;
    QSqlQuery sqlUpdate;
    QSqlQuery sqlSelect;

    //UPDATE table SET valcol = :val WHERE idcol = :pid;
    constStr(SQL_UPDATE1, "UPDATE ")
    constStr(SQL_UPDATE2, " SET ")
    constStr(SQL_UPDATE3, "= :val WHERE ")
    constStr(SQL_UPDATE4, "= :pid;")
    typedef typename templateConcat< SQL_UPDATE1,
        typename templateConcat<table,
        typename templateConcat<SQL_UPDATE2,
        typename templateConcat<valcol,
        typename templateConcat<SQL_UPDATE3,
        typename templateConcat<idcol, SQL_UPDATE4>
        ::Result>::Result>::Result>::Result>::Result>::Result SQL_UPDATE;

    //SELECT valcol FROM table WHERE idcol = :pid LIMIT 1;
    constStr(SQL_SELECT1, "SELECT ")
    constStr(SQL_SELECT2, " FROM ")
    constStr(SQL_SELECT3, " WHERE ")
    constStr(SQL_SELECT4, " = :pid LIMIT 1;")
    typedef typename templateConcat< SQL_SELECT1,
        typename templateConcat<valcol,
        typename templateConcat<SQL_SELECT2,
        typename templateConcat<table,
        typename templateConcat<SQL_SELECT3,
        typename templateConcat<idcol, SQL_SELECT4>
        ::Result>::Result>::Result>::Result>::Result>::Result SQL_SELECT;
};

/******************************************************************************************************/
//Die Definitionen von Template-Methoden muss im Header geschehen, damit man nicht im voraus alle
//benötigten Varianten des Templates angeben muss

template<typename T>
dbpropg<T>::dbpropg()
{

}

template<typename T>
dbpropg<T>::~dbpropg()
{

}

DBP_TEMPLATE
dbprop DBP_TEMPPARA::dbprop() : dbpropg<T>(), initialized(false)
{
}

DBP_TEMPLATE
dbprop DBP_TEMPPARA::dbprop(const dbprop DBP_TEMPPARA &source)
    : parentId(source.parentId),  initialized(source.initialized),
      sqlUpdate(source.sqlUpdate), sqlSelect(source.sqlSelect)
{
}

DBP_TEMPLATE
dbprop DBP_TEMPPARA::~dbprop()
{
}

DBP_TEMPLATE
void dbprop DBP_TEMPPARA::init(QSqlDatabase *database, uint pid)
{
    parentId = pid;

    sqlUpdate = QSqlQuery(*database);
    sqlUpdate.setForwardOnly(true);
    sqlUpdate.prepare( SQL_UPDATE::string() );
    sqlUpdate.bindValue(":pid", parentId);

    sqlSelect = QSqlQuery(*database);
    sqlSelect.setForwardOnly(true);
    sqlSelect.prepare( SQL_SELECT::string() );
    sqlSelect.bindValue(":pid", parentId);

    initialized = true;
}

DBP_TEMPLATE
T dbprop DBP_TEMPPARA::Get()
{
    assert(initialized);

    if(!sqlSelect.exec())
        throw std::runtime_error(qPrintable(
                    QApplication::tr("DB-Fehler beim Lesen von ") + valcol::string() + "\n"
                                     + sqlSelect.lastError().text() + "\n"
                                     + sqlSelect.lastQuery() + "\n"
                                     + sqlSelect.boundValue(":pid").toString()));
    if(!sqlSelect.next())
        throw std::runtime_error(qPrintable(
                    QApplication::tr("DB-Fehler beim Lesen von_ ") + valcol::string() + "\n"
                                     + sqlSelect.lastQuery() + "\n"
                                     + sqlSelect.boundValue(":pid").toString()));

    T value = qvariant_cast<T>(sqlSelect.value(0));
    sqlSelect.finish();
    return value;
}

DBP_TEMPLATE
void dbprop DBP_TEMPPARA::Set(const T& val)
{
    assert(initialized);

    sqlUpdate.bindValue(":val", val);

    if(!sqlUpdate.exec())
        throw std::runtime_error(qPrintable(
                    QApplication::tr("DB-Fehler beim Schreiben von ") + valcol::string() + "\n"
                                     + sqlSelect.lastError().text() + "\n"
                                     + sqlSelect.lastQuery()));
    sqlUpdate.finish();
}

DBP_TEMPLATE
void dbprop DBP_TEMPPARA::Unset()
{assert(initialized);

    sqlUpdate.bindValue(":val", QVariant());

    if(!sqlUpdate.exec())
        throw std::runtime_error(qPrintable(
                    QApplication::tr("DB-Fehler beim Leeren von ") + valcol::string() + "\n"
                                     + sqlSelect.lastError().text() + "\n"
                                     + sqlSelect.lastQuery()));
    sqlUpdate.finish();
}

DBP_TEMPLATE
bool dbprop DBP_TEMPPARA::IsNull()
{
    assert(initialized);

    if(!sqlSelect.exec())
        throw std::runtime_error(qPrintable(
            QApplication::tr("DB-Fehler beim Prüfen von ") + valcol::string() + "\n"
                                     + sqlSelect.lastError().text()));
    bool isNull = sqlSelect.next();
    sqlSelect.finish();
    return isNull;

}


#endif // DBPROP_H

