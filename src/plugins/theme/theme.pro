######################################################################
# Communi
######################################################################

HEADERS += $$PWD/themeinfo.h
HEADERS += $$PWD/themeplugin.h

SOURCES += $$PWD/themeinfo.cpp
SOURCES += $$PWD/themeplugin.cpp

include(../plugin.pri)
include(../../libs/util/util.pri)
include($$SOURCE_ROOT/themes/themes.pri)
