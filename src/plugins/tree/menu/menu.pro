######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += plugin static
TARGET = menuplugin

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
}

DESTDIR = ../../../../plugins
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/menu.h
SOURCES += $$PWD/menu.cpp

include(../../../config.pri)
include(../../../backend.pri)
include(../../../tree/tree.pri)
