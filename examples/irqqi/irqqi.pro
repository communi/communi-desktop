######################################################################
# Communi
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QT += declarative

# Input
SOURCES += main.cpp
OTHER_FILES += irqqi.qml

include(../examples.pri)
include(../core/core.pri)
