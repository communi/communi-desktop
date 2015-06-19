######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

RESOURCES += $$PWD/alert.qrc

HEADERS += $$PWD/alert.h
HEADERS += $$PWD/dock.h

SOURCES += $$PWD/alert.cpp
SOURCES += $$PWD/dock.cpp
OBJECTIVE_SOURCES += $$PWD/dock_mac.mm
