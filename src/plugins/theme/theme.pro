######################################################################
# Communi
######################################################################

HEADERS += $$PWD/themeplugin.h
SOURCES += $$PWD/themeplugin.cpp

include(../plugin.pri)
include(../shared/shared.pri)
include($$SOURCE_ROOT/themes/themes.pri)
