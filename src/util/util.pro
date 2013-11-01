######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = util

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
    qtHaveModule(multimedia):QT += multimedia
} else {
    lessThan(QT_MAJOR_VERSION, 4) | lessThan(QT_MINOR_VERSION, 7) {
        error(Communi2/QtWidgets requires Qt 4.7 or newer but Qt $$[QT_VERSION] was detected.)
    }
    CONFIG(phonon, phonon|no_phonon):QT += phonon
}

DESTDIR = ../../lib
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/commandparser.h
HEADERS += $$PWD/ignoremanager.h
HEADERS += $$PWD/messageformatter.h
HEADERS += $$PWD/sharedtimer.h
HEADERS += $$PWD/soundnotification.h
HEADERS += $$PWD/zncmanager.h

SOURCES += $$PWD/commandparser.cpp
SOURCES += $$PWD/ignoremanager.cpp
SOURCES += $$PWD/messageformatter.cpp
SOURCES += $$PWD/sharedtimer.cpp
SOURCES += $$PWD/soundnotification.cpp
SOURCES += $$PWD/zncmanager.cpp

include(../config.pri)
include(../backend.pri)
include(systemnotifier/systemnotifier.pri)
