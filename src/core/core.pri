######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/commandparser.h
HEADERS += $$PWD/connectioninfo.h
HEADERS += $$PWD/messagehandler.h
HEADERS += $$PWD/session.h
HEADERS += $$PWD/streamer.h
HEADERS += $$PWD/viewinfo.h
HEADERS += $$PWD/zncplayback.h

SOURCES += $$PWD/commandparser.cpp
SOURCES += $$PWD/connectioninfo.cpp
SOURCES += $$PWD/messagehandler.cpp
SOURCES += $$PWD/session.cpp
SOURCES += $$PWD/zncplayback.cpp
