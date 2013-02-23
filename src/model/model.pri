######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/channelitem.h
HEADERS += $$PWD/queryitem.h
HEADERS += $$PWD/serveritem.h
HEADERS += $$PWD/session.h
HEADERS += $$PWD/sessionitem.h
HEADERS += $$PWD/sessionmodel.h
HEADERS += $$PWD/usermodel.h

SOURCES += $$PWD/channelitem.cpp
SOURCES += $$PWD/queryitem.cpp
SOURCES += $$PWD/serveritem.cpp
SOURCES += $$PWD/session.cpp
SOURCES += $$PWD/sessionitem.cpp
SOURCES += $$PWD/sessionmodel.cpp
SOURCES += $$PWD/usermodel.cpp
