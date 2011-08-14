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
SOURCES += src/messageformatter.cpp

include(../examples.pri)
