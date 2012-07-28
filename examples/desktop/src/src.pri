######################################################################
# Communi: src.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/application.h
HEADERS += $$PWD/homepage.h
HEADERS += $$PWD/mainwindow.h

SOURCES += $$PWD/application.cpp
SOURCES += $$PWD/homepage.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/mainwindow.cpp

include(3rdparty/3rdparty.pri)
include(gui/gui.pri)
include(util/util.pri)
include(wizard/wizard.pri)
