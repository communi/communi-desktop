######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static communi_plugin

HEADERS += $$PWD/overlay.h
HEADERS += $$PWD/overlayplugin.h

SOURCES += $$PWD/overlay.cpp
SOURCES += $$PWD/overlayplugin.cpp

RESOURCES += $$PWD/overlay.qrc
