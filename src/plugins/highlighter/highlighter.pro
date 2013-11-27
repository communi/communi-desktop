######################################################################
# Communi
######################################################################

HEADERS += $$PWD/documenthighlighter.h
HEADERS += $$PWD/highlighterplugin.h
HEADERS += $$PWD/treehighlighter.h

SOURCES += $$PWD/documenthighlighter.cpp
SOURCES += $$PWD/highlighterplugin.cpp
SOURCES += $$PWD/treehighlighter.cpp

include(../plugin.pri)
include(../../libs/util/util.pri)
