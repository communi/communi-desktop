######################################################################
# Communi
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
CONFIG -= app_bundle

# Input
HEADERS += ircbot.h
SOURCES += ircbot.cpp main.cpp

include(../examples.pri)
