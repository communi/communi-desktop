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

HEADERS += $$PWD/browserplugin.h
HEADERS += $$PWD/documentplugin.h
HEADERS += $$PWD/inputplugin.h
HEADERS += $$PWD/listplugin.h
HEADERS += $$PWD/listview.h
HEADERS += $$PWD/textbrowser.h
HEADERS += $$PWD/textdocument.h
HEADERS += $$PWD/textinput.h
HEADERS += $$PWD/topiclabel.h
HEADERS += $$PWD/treeitem.h
HEADERS += $$PWD/treeplugin.h
HEADERS += $$PWD/treerole.h
HEADERS += $$PWD/treewidget.h

SOURCES += $$PWD/listview.cpp
SOURCES += $$PWD/textbrowser.cpp
SOURCES += $$PWD/textdocument.cpp
SOURCES += $$PWD/textinput.cpp
SOURCES += $$PWD/topiclabel.cpp
SOURCES += $$PWD/treeitem.cpp
SOURCES += $$PWD/treewidget.cpp

include(../../config.pri)
include(../backend/backend.pri)
include(../util/util.pri)
