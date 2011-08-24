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
OTHER_FILES += irqqi.qml

include(../examples.pri)
