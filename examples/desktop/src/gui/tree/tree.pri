######################################################################
# Communi: tree.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/sessiontreeitem.h
HEADERS += $$PWD/sessiontreewidget.h

SOURCES += $$PWD/sessiontreeitem.cpp
SOURCES += $$PWD/sessiontreewidget.cpp
