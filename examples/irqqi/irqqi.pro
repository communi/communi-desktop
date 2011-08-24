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
unix:PRE_TARGETDEPS += ../core/lib/libCore.a

# Input
SOURCES += main.cpp
OTHER_FILES += irqqi.qml

include(../examples.pri)
