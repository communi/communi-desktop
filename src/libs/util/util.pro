######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = util

DESTDIR = ../../../lib
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/sharedtimer.h
HEADERS += $$PWD/messageformatter.h

SOURCES += $$PWD/messageformatter.cpp
SOURCES += $$PWD/sharedtimer.cpp

include(../../config.pri)
include(../backend/backend.pri)
