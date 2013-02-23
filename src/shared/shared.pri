######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/channelinfo.h
HEADERS += $$PWD/channelitem.h
HEADERS += $$PWD/commandparser.h
HEADERS += $$PWD/connectioninfo.h
HEADERS += $$PWD/messageformatter.h
HEADERS += $$PWD/messagereceiver.h
HEADERS += $$PWD/queryitem.h
HEADERS += $$PWD/serveritem.h
HEADERS += $$PWD/session.h
HEADERS += $$PWD/sessionitem.h
HEADERS += $$PWD/sessionmodel.h
HEADERS += $$PWD/sortedusermodel.h
HEADERS += $$PWD/streamer.h
HEADERS += $$PWD/usermodel.h

SOURCES += $$PWD/channelitem.cpp
SOURCES += $$PWD/commandparser.cpp
SOURCES += $$PWD/messageformatter.cpp
SOURCES += $$PWD/queryitem.cpp
SOURCES += $$PWD/serveritem.cpp
SOURCES += $$PWD/session.cpp
SOURCES += $$PWD/sessionitem.cpp
SOURCES += $$PWD/sessionmodel.cpp
SOURCES += $$PWD/sortedusermodel.cpp
SOURCES += $$PWD/usermodel.cpp
