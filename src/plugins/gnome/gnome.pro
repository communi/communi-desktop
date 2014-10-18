######################################################################
# Communi
######################################################################

TEMPLATE = lib
COMMUNI += core model util
CONFIG += communi_plugin
QT += x11extras

HEADERS += $$PWD/gnomeplugin.h
SOURCES += $$PWD/gnomeplugin.cpp
