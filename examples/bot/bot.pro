######################################################################
# Communi
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += console
CONFIG -= app_bundle
QT = core

# Input
HEADERS += ircbot.h
SOURCES += ircbot.cpp main.cpp

include(../examples.pri)
