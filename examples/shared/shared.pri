######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/commandparser.h
HEADERS += $$PWD/connection.h
HEADERS += $$PWD/messageformatter.h
HEADERS += $$PWD/messagehandler.h
HEADERS += $$PWD/session.h
HEADERS += $$PWD/streamer.h

SOURCES += $$PWD/commandparser.cpp
SOURCES += $$PWD/connection.cpp
SOURCES += $$PWD/messageformatter.cpp
SOURCES += $$PWD/messagehandler.cpp
SOURCES += $$PWD/session.cpp
