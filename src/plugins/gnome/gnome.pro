######################################################################
# Communi
######################################################################

TEMPLATE = lib
COMMUNI += core model util
CONFIG += communi_plugin
QT += x11extras
CONFIG += c++11

HEADERS += $$PWD/gnomeplugin.h
HEADERS += $$PWD/x11helper.h

SOURCES += $$PWD/gnomeplugin.cpp
SOURCES += $$PWD/x11helper.cpp

packagesExist(gio-2.0) {
    PKGCONFIG += gio-2.0
    CONFIG += link_pkgconfig
    DEFINES += COMMUNI_HAVE_GIO
    HEADERS += $$PWD/gnomemenu.h
    SOURCES += $$PWD/gnomemenu.cpp
}
