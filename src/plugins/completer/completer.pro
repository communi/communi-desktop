######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += plugin static
TARGET = completerplugin

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
}

DESTDIR = ../../../plugins
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/completer.h
HEADERS += $$PWD/completerplugin.h

SOURCES += $$PWD/completer.cpp
SOURCES += $$PWD/completerplugin.cpp

include(../../config.pri)
include(../../libs/api/api.pri)
include(../../libs/backend/backend.pri)
