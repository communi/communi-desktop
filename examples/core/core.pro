######################################################################
# Communi
######################################################################

TEMPLATE = lib
TARGET = Core
DESTDIR = lib
DEPENDPATH += . include src
INCLUDEPATH += . include
CONFIG += static
QT = core

# Input
HEADERS += include/commandparser.h
HEADERS += include/messageformatter.h
HEADERS += include/messagehandler.h

SOURCES += src/commandparser.cpp
SOURCES += src/messageformatter.cpp
SOURCES += src/messagehandler.cpp

include(../examples.pri)
