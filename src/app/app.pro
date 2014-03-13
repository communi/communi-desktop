######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi
CONFIG += communi_config communi_3rdparty communi_backend communi_base

DESTDIR = ../../bin
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

mac:LIBS += -framework SystemConfiguration
else:win32:LIBS += -lole32
else:unix:QT += dbus

greaterThan(QT_MAJOR_VERSION, 4) {
    qtHaveModule(multimedia):QT += multimedia
} else {
    CONFIG(phonon, phonon|no_phonon):QT += phonon
}

win32:target.path = $$[QT_INSTALL_BINS]
else:mac:target.path = /Applications
else:target.path = /usr/bin
INSTALLS += target

FORMS += $$PWD/connectpage.ui
FORMS += $$PWD/settingspage.ui

HEADERS += $$PWD/chatpage.h
HEADERS += $$PWD/connectpage.h
HEADERS += $$PWD/mainwindow.h
HEADERS += $$PWD/pluginloader.h
HEADERS += $$PWD/settingspage.h
HEADERS += $$PWD/splitview.h
HEADERS += $$PWD/themeinfo.h
HEADERS += $$PWD/themeloader.h

SOURCES += $$PWD/chatpage.cpp
SOURCES += $$PWD/connectpage.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/mainwindow.cpp
SOURCES += $$PWD/pluginloader.cpp
SOURCES += $$PWD/settingspage.cpp
SOURCES += $$PWD/splitview.cpp
SOURCES += $$PWD/themeinfo.cpp
SOURCES += $$PWD/themeloader.cpp

STATIC_PLUGINS += commander
STATIC_PLUGINS += dock
STATIC_PLUGINS += ignore
STATIC_PLUGINS += importer
STATIC_PLUGINS += monitor
STATIC_PLUGINS += overlay
STATIC_PLUGINS += sound
STATIC_PLUGINS += tray
STATIC_PLUGINS += verifier
STATIC_PLUGINS += znc

for(PLUGIN, STATIC_PLUGINS) {
    communiAddStaticPlugin(PLUGIN)
}

include(finder/finder.pri)
include(../../resources/resources.pri)
include(../../themes/themes.pri)
