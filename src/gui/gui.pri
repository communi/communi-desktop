######################################################################
# Communi: gui.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

FORMS += $$PWD/bufferview.ui

HEADERS += $$PWD/bufferview.h
HEADERS += $$PWD/itemdelegate.h
HEADERS += $$PWD/lineeditor.h
HEADERS += $$PWD/messagestackview.h
HEADERS += $$PWD/searcheditor.h
HEADERS += $$PWD/sessionstackview.h
HEADERS += $$PWD/treeitem.h
HEADERS += $$PWD/treemenu.h
HEADERS += $$PWD/treewidget.h
HEADERS += $$PWD/userlistmenu.h
HEADERS += $$PWD/userlistview.h

SOURCES += $$PWD/bufferview.cpp
SOURCES += $$PWD/itemdelegate.cpp
SOURCES += $$PWD/lineeditor.cpp
SOURCES += $$PWD/messagestackview.cpp
SOURCES += $$PWD/searcheditor.cpp
SOURCES += $$PWD/sessionstackview.cpp
SOURCES += $$PWD/treeitem.cpp
SOURCES += $$PWD/treemenu.cpp
SOURCES += $$PWD/treewidget.cpp
SOURCES += $$PWD/userlistmenu.cpp
SOURCES += $$PWD/userlistview.cpp

include(util/util.pri)
include(settings/settings.pri)
include(3rdparty/3rdparty.pri)
