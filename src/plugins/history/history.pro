######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += plugin static
TARGET = historyplugin

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
}

DESTDIR = ../../../plugins
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/historyplugin.h
HEADERS += $$PWD/inputhistory.h

SOURCES += $$PWD/historyplugin.cpp
SOURCES += $$PWD/inputhistory.cpp

include(../../config.pri)
include(../../input/input.pri)
