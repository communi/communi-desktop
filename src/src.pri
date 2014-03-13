######################################################################
# Communi: src.pri
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

exists(backend/src/src.pri) {
    include(backend/src/src.pri)
} else:load(communi-config, true) {
    # bool load(feature, ignore_error)
    CONFIG += communi
    COMMUNI += core model util
} else {
    error(Cannot detect libcommuni. Either install it or run \'git submodule update --init\' in $${_PRO_FILE_PWD_}.)
}

!exists(3rdparty/qtsystemnotifier/systemnotifier.pri) {
    error(3rdparty/qtsystemnotifier submodule is missing. Run \'git submodule update --init\' in $${_PRO_FILE_PWD_}.)
}

include(gui/gui.pri)
include(core/core.pri)
include(shared/shared.pri)
include(wizard/wizard.pri)
include(3rdparty/qtsystemnotifier/systemnotifier.pri)

# TODO:
include(libs/base/base.pri)
include(libs/3rdparty/3rdparty.pri)
