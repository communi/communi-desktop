######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/channelinfo.h
HEADERS += $$PWD/commandparser.h
HEADERS += $$PWD/connectioninfo.h
HEADERS += $$PWD/messageformatter.h
HEADERS += $$PWD/streamer.h

SOURCES += $$PWD/commandparser.cpp
SOURCES += $$PWD/messageformatter.cpp
