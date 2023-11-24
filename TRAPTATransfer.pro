TARGET = TRAPTATransfer
TEMPLATE = app

QT       += core gui widgets
CONFIG += c++11
CONFIG += release

debug {
    DESTDIR = bin/debug
    OBJECTS_DIR = build/lib/debug
}
release {
    DESTDIR = bin/release
    OBJECTS_DIR = build/lib/release
}

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
