######################################################################
# Communi: command.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/command.h
HEADERS += $$PWD/commandengine.h
HEADERS += $$PWD/commandxmlreader.h
HEADERS += $$PWD/commandxmlwriter.h

SOURCES += $$PWD/command.cpp
SOURCES += $$PWD/commandengine.cpp
SOURCES += $$PWD/commandxmlreader.cpp
SOURCES += $$PWD/commandxmlwriter.cpp
