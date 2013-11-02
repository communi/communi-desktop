######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = base

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
}

DESTDIR = ../../../lib
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/inputplugin.h
HEADERS += $$PWD/textinput.h
HEADERS += $$PWD/treeitem.h
HEADERS += $$PWD/treeplugin.h
HEADERS += $$PWD/treerole.h
HEADERS += $$PWD/treewidget.h

SOURCES += $$PWD/textinput.cpp
SOURCES += $$PWD/treeitem.cpp
SOURCES += $$PWD/treewidget.cpp

include(../../config.pri)
