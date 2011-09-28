######################################################################
# Communi
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QT += declarative network

# Input
SOURCES += main.cpp
OTHER_FILES += irqqi.qml Dialog.qml Page.qml

include(../examples.pri)
include(../shared/shared.pri)
