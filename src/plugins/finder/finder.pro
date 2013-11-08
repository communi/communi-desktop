######################################################################
# Communi
######################################################################

HEADERS += $$PWD/finderplugin.h
HEADERS += $$PWD/searchinput.h
HEADERS += $$PWD/searchpopup.h

SOURCES += $$PWD/finderplugin.cpp
SOURCES += $$PWD/searchinput.cpp
SOURCES += $$PWD/searchpopup.cpp

RESOURCES += finder.qrc

include(../plugin.pri)
