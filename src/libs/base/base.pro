######################################################################
# Communi
######################################################################

TEMPLATE = lib
TARGET = Communi
CONFIG += communi
COMMUNI += core model util

DESTDIR = $$BUILD_TREE/lib
DLLDESTDIR = $$BUILD_TREE/bin
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
DEFINES += BUILD_BASE

!mac {
    load(communi_installs.prf)
    isEmpty(COMMUNI_INSTALL_LIBS):error(COMMUNI_INSTALL_LIBS empty!)
    isEmpty(COMMUNI_INSTALL_BINS):error(COMMUNI_INSTALL_BINS empty!)

    target.path = $$COMMUNI_INSTALL_LIBS
    dlltarget.path = $$COMMUNI_INSTALL_BINS
    INSTALLS += target dlltarget
}

HEADERS += $$PWD/bufferview.h
HEADERS += $$PWD/eventformatter.h
HEADERS += $$PWD/listview.h
HEADERS += $$PWD/messagedata.h
HEADERS += $$PWD/messageformatter.h
HEADERS += $$PWD/textbrowser.h
HEADERS += $$PWD/textdocument.h
HEADERS += $$PWD/textinput.h
HEADERS += $$PWD/themeinfo.h
HEADERS += $$PWD/titlebar.h

SOURCES += $$PWD/bufferview.cpp
SOURCES += $$PWD/eventformatter.cpp
SOURCES += $$PWD/listview.cpp
SOURCES += $$PWD/messagedata.cpp
SOURCES += $$PWD/messageformatter.cpp
SOURCES += $$PWD/textbrowser.cpp
SOURCES += $$PWD/textdocument.cpp
SOURCES += $$PWD/textinput.cpp
SOURCES += $$PWD/themeinfo.cpp
SOURCES += $$PWD/titlebar.cpp

include(shared/shared.pri)
include(plugins/plugins.pri)

mac: QMAKE_SONAME_PREFIX = @rpath
