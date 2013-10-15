######################################################################
# Communi: util.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/commandparser.h
HEADERS += $$PWD/ignoremanager.h
HEADERS += $$PWD/messageformatter.h
HEADERS += $$PWD/sharedtimer.h
HEADERS += $$PWD/soundnotification.h
HEADERS += $$PWD/zncmanager.h

SOURCES += $$PWD/commandparser.cpp
SOURCES += $$PWD/ignoremanager.cpp
SOURCES += $$PWD/messageformatter.cpp
SOURCES += $$PWD/sharedtimer.cpp
SOURCES += $$PWD/soundnotification.cpp
SOURCES += $$PWD/zncmanager.cpp

include(systemnotifier/systemnotifier.pri)
