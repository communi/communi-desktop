######################################################################
# Communi
######################################################################

TEMPLATE = lib
TARGET = base
CONFIG += static communi
COMMUNI += core model util

DESTDIR = $$BUILD_TREE/lib
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/bufferview.h
HEADERS += $$PWD/listview.h
HEADERS += $$PWD/textbrowser.h
HEADERS += $$PWD/textdocument.h
HEADERS += $$PWD/textinput.h
HEADERS += $$PWD/titlebar.h

SOURCES += $$PWD/bufferview.cpp
SOURCES += $$PWD/listview.cpp
SOURCES += $$PWD/textbrowser.cpp
SOURCES += $$PWD/textdocument.cpp
SOURCES += $$PWD/textinput.cpp
SOURCES += $$PWD/titlebar.cpp

include(shared/shared.pri)
include(plugins/plugins.pri)
