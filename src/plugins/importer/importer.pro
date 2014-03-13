######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static communi_plugin

HEADERS += $$PWD/importerplugin.h
SOURCES += $$PWD/importerplugin.cpp

include(compat/compat.pri)
