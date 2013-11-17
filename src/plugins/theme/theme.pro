######################################################################
# Communi
######################################################################

HEADERS += $$PWD/stylesheet.h
HEADERS += $$PWD/themeplugin.h

SOURCES += $$PWD/stylesheet.cpp
SOURCES += $$PWD/themeplugin.cpp

include(../plugin.pri)
include(../shared/shared.pri)
include($$SOURCE_ROOT/themes/themes.pri)
