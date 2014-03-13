######################################################################
# Communi: gui.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/messagestackview.h
HEADERS += $$PWD/searcheditor.h
HEADERS += $$PWD/sessionstackview.h
HEADERS += $$PWD/treemenu.h

SOURCES += $$PWD/messagestackview.cpp
SOURCES += $$PWD/searcheditor.cpp
SOURCES += $$PWD/sessionstackview.cpp
SOURCES += $$PWD/treemenu.cpp

include(util/util.pri)
include(settings/settings.pri)
