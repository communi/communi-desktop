######################################################################
# Communi: 3rdparty.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

win32:HEADERS += $$PWD/qtwin.h

win32:SOURCES += $$PWD/qtwin.cpp
