######################################################################
# Communi
######################################################################

HEADERS += $$PWD/browserfinder.h
HEADERS += $$PWD/finder.h
HEADERS += $$PWD/finderplugin.h
HEADERS += $$PWD/treefinder.h

SOURCES += $$PWD/browserfinder.cpp
SOURCES += $$PWD/finder.cpp
SOURCES += $$PWD/finderplugin.cpp
SOURCES += $$PWD/treefinder.cpp

RESOURCES += finder.qrc

include(../plugin.pri)
