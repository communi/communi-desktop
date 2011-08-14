######################################################################
# Communi
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QT += declarative

INCLUDEPATH += ../core/include
LIBS += -L../core/lib -lCore

# Input
SOURCES += main.cpp

include(../examples.pri)
