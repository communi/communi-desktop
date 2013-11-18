######################################################################
# Communi
######################################################################

HEADERS += $$PWD/theme.h
HEADERS += $$PWD/themeplugin.h

SOURCES += $$PWD/theme.cpp
SOURCES += $$PWD/themeplugin.cpp

include(../plugin.pri)
include(../shared/shared.pri)
include($$SOURCE_ROOT/themes/themes.pri)
