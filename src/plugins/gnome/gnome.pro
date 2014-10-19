######################################################################
# Communi
######################################################################

TEMPLATE = lib
COMMUNI += core model util
CONFIG += communi_plugin
QT += x11extras

HEADERS += $$PWD/gnomeplugin.h
HEADERS += $$PWD/x11helper.h

SOURCES += $$PWD/gnomeplugin.cpp
SOURCES += $$PWD/x11helper.cpp
