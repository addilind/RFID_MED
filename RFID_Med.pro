#-------------------------------------------------
#
# Project created by QtCreator 2015-12-14T13:28:24
#
#-------------------------------------------------

QT       += core gui sql serialport widgets
CONFIG   += c++14

TARGET = RFID_Med
TEMPLATE = app


SOURCES += main.cpp\
        homestation.cpp \
    datastore.cpp \
    reader.cpp \
    settings.cpp \
    Model/medication.cpp \
    Model/unit.cpp \
    simulator.cpp \
    Model/medsettings.cpp \
    Model/unitsettings.cpp \
    settingstagassign.cpp \
    Model/medlist.cpp

HEADERS  += homestation.h \
    datastore.h \
    reader.h \
    settings.h \
    Model/medication.h \
    Model/dbprop.h \
    Model/unit.h \
    simulator.h \
    Model/medsettings.h \
    Model/unitsettings.h \
    settingstagassign.h \
    Model/medlist.h

FORMS    += homestation.ui \
    settings.ui \
    simulator.ui \
    settingstagassign.ui

RESOURCES += \
    resources.qrc
