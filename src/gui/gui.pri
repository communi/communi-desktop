######################################################################
# Communi: gui.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

FORMS += $$PWD/messageview.ui

HEADERS += $$PWD/itemdelegate.h
HEADERS += $$PWD/lineeditor.h
HEADERS += $$PWD/menufactory.h
HEADERS += $$PWD/messagestackview.h
HEADERS += $$PWD/messageview.h
HEADERS += $$PWD/searcheditor.h
HEADERS += $$PWD/sessionstackview.h
HEADERS += $$PWD/sessiontreeitem.h
HEADERS += $$PWD/sessiontreemenu.h
HEADERS += $$PWD/sessiontreewidget.h
HEADERS += $$PWD/userlistview.h

SOURCES += $$PWD/itemdelegate.cpp
SOURCES += $$PWD/lineeditor.cpp
SOURCES += $$PWD/menufactory.cpp
SOURCES += $$PWD/messagestackview.cpp
SOURCES += $$PWD/messageview.cpp
SOURCES += $$PWD/searcheditor.cpp
SOURCES += $$PWD/sessionstackview.cpp
SOURCES += $$PWD/sessiontreeitem.cpp
SOURCES += $$PWD/sessiontreemenu.cpp
SOURCES += $$PWD/sessiontreewidget.cpp
SOURCES += $$PWD/userlistview.cpp

include(util/util.pri)
include(settings/settings.pri)
include(3rdparty/3rdparty.pri)
