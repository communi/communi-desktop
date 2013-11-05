######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/commandparser.h
HEADERS += $$PWD/ignoremanager.h
HEADERS += $$PWD/zncmanager.h

SOURCES += $$PWD/commandparser.cpp
SOURCES += $$PWD/ignoremanager.cpp
SOURCES += $$PWD/zncmanager.cpp

include(systemnotifier/systemnotifier.pri)
