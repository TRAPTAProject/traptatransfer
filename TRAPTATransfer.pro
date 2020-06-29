#-------------------------------------------------
#
# Project created by QtCreator 2013-11-23T13:54:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TRAPTATransfer
TEMPLATE = app


SOURCES += main.cpp\
        ratransfert.cpp \
    wdrequest.cpp \
    mystandarditemmodel.cpp \
    sqlscript.cpp

HEADERS  += ratransfert.h \
    wdrequest.h \
    mystandarditemmodel.h \
    sqlscript.h

FORMS    += ratransfert.ui

RC_FILE = traptatransfer.rc

RESOURCES += \
    resources.qrc
