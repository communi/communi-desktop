######################################################################
# Communi
######################################################################

TEMPLATE = lib
TARGET = base
CONFIG += static communi_config communi_backend

DESTDIR = $$BUILD_ROOT/lib
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/plugins/bufferplugin.h
HEADERS += $$PWD/plugins/bufferviewplugin.h
HEADERS += $$PWD/plugins/connectionplugin.h
HEADERS += $$PWD/plugins/textdocumentplugin.h
HEADERS += $$PWD/plugins/treewidgetplugin.h

HEADERS += $$PWD/bufferview.h
HEADERS += $$PWD/listview.h
HEADERS += $$PWD/textbrowser.h
HEADERS += $$PWD/textdocument.h
HEADERS += $$PWD/textinput.h
HEADERS += $$PWD/titlebar.h
HEADERS += $$PWD/treedelegate.h
HEADERS += $$PWD/treeitem.h
HEADERS += $$PWD/treerole.h
HEADERS += $$PWD/treewidget.h

SOURCES += $$PWD/bufferview.cpp
SOURCES += $$PWD/listview.cpp
SOURCES += $$PWD/textbrowser.cpp
SOURCES += $$PWD/textdocument.cpp
SOURCES += $$PWD/textinput.cpp
SOURCES += $$PWD/titlebar.cpp
SOURCES += $$PWD/treedelegate.cpp
SOURCES += $$PWD/treeitem.cpp
SOURCES += $$PWD/treewidget.cpp

!exists(shared/shared.pri) {
    error(A Git submodule is missing. Run \'git submodule update --init\' in $${SOURCE_ROOT}.)
}
include(shared/shared.pri)
