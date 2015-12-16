#-------------------------------------------------
#
# Project created by QtCreator 2015-12-14T13:28:24
#
#-------------------------------------------------

QT       += core gui sql serialport
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RFID_Med
TEMPLATE = app


SOURCES += main.cpp\
        homestation.cpp \
    datastore.cpp \
    reader.cpp

HEADERS  += homestation.h \
    datastore.h \
    reader.h

FORMS    += homestation.ui

RESOURCES += \
    resources.qrc
