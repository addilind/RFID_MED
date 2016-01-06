#include "homestation.h"
#include <iostream>
#include <QApplication>
#include <QCommandLineParser>

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
                          "/dev/ttyUSB0" //Standardwert
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

    if(parser.isSet(help))
    {
        std::cout << qPrintable( parser.helpText() ) << std::endl;
        return false; //Nach Hilfetext beenden
    }

    home.SetSerialPort(parser.value(portOption)); //Falls nicht übergeben werden oben gesetzte Standardwerte gesetzt
    home.SetDBFile(parser.value(dbOption));

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("RFID-Med");

    Homestation home;

    if(!parseCommandline(a, home))
        return 0;

    home.show();

    return a.exec();
}
