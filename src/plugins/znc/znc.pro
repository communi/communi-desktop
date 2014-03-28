######################################################################
# Communi
######################################################################

TEMPLATE = lib
COMMUNI += core model util
CONFIG += communi_plugin
!mac:CONFIG += static

HEADERS += $$PWD/zncplugin.h
SOURCES += $$PWD/zncplugin.cpp
