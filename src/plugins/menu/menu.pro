######################################################################
# Communi
######################################################################

HEADERS += $$PWD/browsermenu.h
HEADERS += $$PWD/listmenu.h
HEADERS += $$PWD/menuplugin.h
HEADERS += $$PWD/titlemenu.h
HEADERS += $$PWD/treemenu.h

SOURCES += $$PWD/browsermenu.cpp
SOURCES += $$PWD/listmenu.cpp
SOURCES += $$PWD/menuplugin.cpp
SOURCES += $$PWD/titlemenu.cpp
SOURCES += $$PWD/treemenu.cpp

include(../plugin.pri)
