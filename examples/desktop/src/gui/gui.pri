######################################################################
# Communi: gui.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

FORMS += $$PWD/messageview.ui

HEADERS += $$PWD/lineeditor.h
HEADERS += $$PWD/multisessiontabwidget.h
HEADERS += $$PWD/messageview.h
HEADERS += $$PWD/searcheditor.h
HEADERS += $$PWD/sessiontabwidget.h
HEADERS += $$PWD/settings.h
HEADERS += $$PWD/sortedusermodel.h

SOURCES += $$PWD/lineeditor.cpp
SOURCES += $$PWD/multisessiontabwidget.cpp
SOURCES += $$PWD/messageview.cpp
SOURCES += $$PWD/searcheditor.cpp
SOURCES += $$PWD/sessiontabwidget.cpp
SOURCES += $$PWD/settings.cpp
SOURCES += $$PWD/sortedusermodel.cpp

include(util/util.pri)
include(3rdparty/3rdparty.pri)
