######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/connection.h
HEADERS += $$PWD/connectioninfo.h
HEADERS += $$PWD/messagehandler.h
HEADERS += $$PWD/streamer.h
HEADERS += $$PWD/viewinfo.h

SOURCES += $$PWD/connection.cpp
SOURCES += $$PWD/connectioninfo.cpp
SOURCES += $$PWD/messagehandler.cpp
