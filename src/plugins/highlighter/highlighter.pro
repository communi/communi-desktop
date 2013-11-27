######################################################################
# Communi
######################################################################

HEADERS += $$PWD/documenthighlighter.h
HEADERS += $$PWD/highlighterplugin.h

SOURCES += $$PWD/documenthighlighter.cpp
SOURCES += $$PWD/highlighterplugin.cpp

include(../plugin.pri)
include(../../libs/util/util.pri)
