######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/connectioninfo.h
HEADERS += $$PWD/messagehandler.h
HEADERS += $$PWD/session.h
HEADERS += $$PWD/streamer.h
HEADERS += $$PWD/viewinfo.h

SOURCES += $$PWD/connectioninfo.cpp
SOURCES += $$PWD/messagehandler.cpp
SOURCES += $$PWD/session.cpp
