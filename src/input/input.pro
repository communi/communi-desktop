######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = input

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
}

DESTDIR = ../../lib
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/inputplugin.h
HEADERS += $$PWD/lineeditor.h
HEADERS += $$PWD/textinput.h

SOURCES += $$PWD/lineeditor.cpp
SOURCES += $$PWD/textinput.cpp

include(../config.pri)
include(../backend.pri)
include(../util/util.pri)
include(../3rdparty/3rdparty.pri)
