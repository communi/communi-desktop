######################################################################
# Communi
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

QT = core network
CONFIG += console no_keywords
CONFIG += communi

# Input
HEADERS += session.h
SOURCES += session.cpp main.cpp
