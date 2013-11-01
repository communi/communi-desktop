######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = view

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
}

DESTDIR = ../../lib
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/browser.h
HEADERS += $$PWD/bufferview.h
HEADERS += $$PWD/document.h
HEADERS += $$PWD/editablelabel.h
HEADERS += $$PWD/itemdelegate.h
HEADERS += $$PWD/searchentry.h
HEADERS += $$PWD/textbrowser.h
HEADERS += $$PWD/textdocument.h
HEADERS += $$PWD/topiclabel.h
HEADERS += $$PWD/userlistview.h

SOURCES += $$PWD/browser.cpp
SOURCES += $$PWD/bufferview.cpp
SOURCES += $$PWD/document.cpp
SOURCES += $$PWD/editablelabel.cpp
SOURCES += $$PWD/itemdelegate.cpp
SOURCES += $$PWD/searchentry.cpp
SOURCES += $$PWD/textbrowser.cpp
SOURCES += $$PWD/textdocument.cpp
SOURCES += $$PWD/topiclabel.cpp
SOURCES += $$PWD/userlistview.cpp

include(../config.pri)
include(../backend.pri)
include(../util/util.pri)
include(../input/input.pri)
include(../3rdparty/3rdparty.pri)
