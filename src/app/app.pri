######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/addviewdialog.h
HEADERS += $$PWD/application.h
HEADERS += $$PWD/helppopup.h
HEADERS += $$PWD/homepage.h
HEADERS += $$PWD/mainwindow.h
HEADERS += $$PWD/overlay.h
HEADERS += $$PWD/searchpopup.h
HEADERS += $$PWD/soundnotification.h
HEADERS += $$PWD/toolbar.h
HEADERS += $$PWD/trayicon.h

SOURCES += $$PWD/addviewdialog.cpp
SOURCES += $$PWD/application.cpp
SOURCES += $$PWD/helppopup.cpp
SOURCES += $$PWD/homepage.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/mainwindow.cpp
SOURCES += $$PWD/overlay.cpp
SOURCES += $$PWD/searchpopup.cpp
SOURCES += $$PWD/soundnotification.cpp
SOURCES += $$PWD/toolbar.cpp
SOURCES += $$PWD/trayicon.cpp

include(gui/gui.pri)
include(core/core.pri)
include(wizard/wizard.pri)
