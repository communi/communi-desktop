######################################################################
# Communi: 3rdparty.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/fancylineedit.h
win32:HEADERS += $$PWD/qtwin.h

SOURCES += $$PWD/fancylineedit.cpp
win32:SOURCES += $$PWD/qtwin.cpp
