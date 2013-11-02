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

HEADERS += $$PWD/history.h
HEADERS += $$PWD/historyplugin.h

SOURCES += $$PWD/history.cpp
SOURCES += $$PWD/historyplugin.cpp

include(../../config.pri)
include(../../libs/input/input.pri)
