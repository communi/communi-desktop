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

DESTDIR = $$BUILD_ROOT/lib

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/bufferview.h
HEADERS += $$PWD/listview.h
HEADERS += $$PWD/splitview.h
HEADERS += $$PWD/syntaxhighlighter.h
HEADERS += $$PWD/textbrowser.h
HEADERS += $$PWD/textdocument.h
HEADERS += $$PWD/textinput.h
HEADERS += $$PWD/titlebar.h
HEADERS += $$PWD/treeitem.h
HEADERS += $$PWD/treewidget.h
HEADERS += $$PWD/window.h

SOURCES += $$PWD/bufferview.cpp
SOURCES += $$PWD/listview.cpp
SOURCES += $$PWD/splitview.cpp
SOURCES += $$PWD/syntaxhighlighter.cpp
SOURCES += $$PWD/textbrowser.cpp
SOURCES += $$PWD/textdocument.cpp
SOURCES += $$PWD/textinput.cpp
SOURCES += $$PWD/titlebar.cpp
SOURCES += $$PWD/treeitem.cpp
SOURCES += $$PWD/treewidget.cpp
SOURCES += $$PWD/window.cpp

include(../../config.pri)
include(plugins/plugins.pri)
include(../backend/backend.pri)
include(../util/util.pri)
