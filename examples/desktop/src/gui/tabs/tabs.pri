######################################################################
# Communi: tabs.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/multisessiontabwidget.h
HEADERS += $$PWD/sessiontabwidget.h
HEADERS += $$PWD/tabwidget.h
HEADERS += $$PWD/tabwidget_p.h

SOURCES += $$PWD/multisessiontabwidget.cpp
SOURCES += $$PWD/sessiontabwidget.cpp
SOURCES += $$PWD/tabwidget.cpp
