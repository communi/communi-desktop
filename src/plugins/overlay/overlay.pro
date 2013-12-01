######################################################################
# Communi
######################################################################

HEADERS += $$PWD/overlay.h
HEADERS += $$PWD/overlayplugin.h

SOURCES += $$PWD/overlay.cpp
SOURCES += $$PWD/overlayplugin.cpp

RESOURCES += $$PWD/overlay.qrc

include(../plugin.pri)
