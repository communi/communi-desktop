######################################################################
# Communi: tree.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/sessiontreeitem.h
HEADERS += $$PWD/sessiontreewidget.h
HEADERS += $$PWD/treewidget.h

SOURCES += $$PWD/sessiontreeitem.cpp
SOURCES += $$PWD/sessiontreewidget.cpp
SOURCES += $$PWD/treewidget.cpp
