######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD/include
INCLUDEPATH += $$PWD/include

HEADERS += $$PWD/include/commandparser.h
HEADERS += $$PWD/include/messageformatter.h
HEADERS += $$PWD/include/messagehandler.h
HEADERS += $$PWD/include/session.h

SOURCES += $$PWD/src/commandparser.cpp
SOURCES += $$PWD/src/messageformatter.cpp
SOURCES += $$PWD/src/messagehandler.cpp
SOURCES += $$PWD/src/session.cpp
