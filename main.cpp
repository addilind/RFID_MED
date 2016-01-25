#include "homestation.h"
#include <iostream>
#include <QApplication>
#include <QCommandLineParser>
#include <limits>

bool parseCommandline(QApplication& app, Homestation& home)
//Returnt true, wenn Programm ausgeführt werden soll
{
    QCommandLineParser parser;
    parser.setApplicationDescription("RFID Med");
    auto help = parser.addHelpOption(); //Hilfe-Option automatisch erstellen

    QCommandLineOption portOption (
                          QStringList() << "p" << "port", //Optionsnamen
                          QApplication::tr("Serieller Port, mit dem das Lesegerät verbunden ist"), //Beschreibung
                          QApplication::tr("Port"), //Parametername
                          "COM1" //Standardwert
                      );
    parser.addOption(portOption);

    QCommandLineOption dbOption (
                          QStringList() << "d" << "db", //Optionsnamen
                          QApplication::tr("Datenbankname"), //Beschreibung
                          QApplication::tr("Dateiname"), //Parametername
                          "rfidmed.sqlite" //Standardwert
                      );
    parser.addOption(dbOption);

    parser.process(app);
    home.SetSerialPort(parser.value(portOption)); //Falls nicht übergeben werden oben gesetzte Standardwerte gesetzt
    home.SetDBFile(parser.value(dbOption));

    return true;
}

int main(int argc, char *argv[])
{
    static_assert(std::numeric_limits<unsigned int>::max() >= 0xFFFFFFFFU, "UINT on this platform is not able to hold 32bit!");
    QApplication a(argc, argv);
    QApplication::setApplicationName("RFID-Med");

    Homestation home;

    if(!parseCommandline(a, home))
        return 0;

    home.show();

    return a.exec();
}
