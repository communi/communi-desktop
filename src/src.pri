######################################################################
# Communi: src.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/addviewdialog.h
HEADERS += $$PWD/application.h
HEADERS += $$PWD/homepage.h
HEADERS += $$PWD/mainwindow.h
HEADERS += $$PWD/overlay.h
HEADERS += $$PWD/searchpopup.h
HEADERS += $$PWD/toolbar.h
HEADERS += $$PWD/trayicon.h

SOURCES += $$PWD/addviewdialog.cpp
SOURCES += $$PWD/application.cpp
SOURCES += $$PWD/homepage.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/mainwindow.cpp
SOURCES += $$PWD/overlay.cpp
SOURCES += $$PWD/searchpopup.cpp
SOURCES += $$PWD/toolbar.cpp
SOURCES += $$PWD/trayicon.cpp

include(gui/gui.pri)
include(shared/shared.pri)
include(wizard/wizard.pri)
include(3rdparty/qtdocktile/qtdocktile.pri)
include(3rdparty/qtsystemnotifier/systemnotifier.pri)
