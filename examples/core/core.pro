######################################################################
# Communi
######################################################################

TEMPLATE = lib
TARGET = Core
DESTDIR = lib
DEPENDPATH += . include src
INCLUDEPATH += . include
CONFIG += static

# Input
HEADERS += include/messageformatter.h
HEADERS += include/messagehandler.h

SOURCES += src/messageformatter.cpp
SOURCES += src/messagehandler.cpp

include(../examples.pri)
