######################################################################
# Communi: gui.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/itemdelegate.h
HEADERS += $$PWD/messagestackview.h
HEADERS += $$PWD/searcheditor.h
HEADERS += $$PWD/sessionstackview.h
HEADERS += $$PWD/treemenu.h
HEADERS += $$PWD/userlistmenu.h

SOURCES += $$PWD/itemdelegate.cpp
SOURCES += $$PWD/messagestackview.cpp
SOURCES += $$PWD/searcheditor.cpp
SOURCES += $$PWD/sessionstackview.cpp
SOURCES += $$PWD/treemenu.cpp
SOURCES += $$PWD/userlistmenu.cpp

include(util/util.pri)
include(settings/settings.pri)
include(3rdparty/3rdparty.pri)
