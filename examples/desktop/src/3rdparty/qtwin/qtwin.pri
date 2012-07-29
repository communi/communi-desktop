######################################################################
# Communi: qtwin.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

win32 {
    HEADERS += $$PWD/qtwin.h
    SOURCES += $$PWD/qtwin.cpp
}
