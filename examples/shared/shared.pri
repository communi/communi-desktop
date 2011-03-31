######################################################################
# Communi
######################################################################

QT = core network
CONFIG += console no_keywords
CONFIG += communi

INCLUDEPATH += ../shared
DEPENDPATH  += ../shared

HEADERS += session.h
SOURCES += session.cpp
