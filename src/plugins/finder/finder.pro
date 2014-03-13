######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static communi_plugin

HEADERS += $$PWD/browserfinder.h
HEADERS += $$PWD/finder.h
HEADERS += $$PWD/finderplugin.h
HEADERS += $$PWD/listfinder.h
HEADERS += $$PWD/treefinder.h

SOURCES += $$PWD/browserfinder.cpp
SOURCES += $$PWD/finder.cpp
SOURCES += $$PWD/finderplugin.cpp
SOURCES += $$PWD/listfinder.cpp
SOURCES += $$PWD/treefinder.cpp
