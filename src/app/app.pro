######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi
CONFIG += communi_config communi_3rdparty communi_backend communi_base

DESTDIR = ../../bin

mac:LIBS += -framework SystemConfiguration
else:win32:LIBS += -lole32
else:unix:QT += dbus

greaterThan(QT_MAJOR_VERSION, 4) {
    qtHaveModule(multimedia):QT += multimedia
} else {
    lessThan(QT_MAJOR_VERSION, 4) | lessThan(QT_MINOR_VERSION, 7) {
        error(Communi requires Qt 4.7 or newer but Qt $$[QT_VERSION] was detected.)
    }
    CONFIG(phonon, phonon|no_phonon):QT += phonon
}

win32:target.path = $$[QT_INSTALL_BINS]
else:mac:target.path = /Applications
else:target.path = /usr/bin
INSTALLS += target

FORMS += $$PWD/connectpage.ui
FORMS += $$PWD/settingspage.ui

HEADERS += $$PWD/appwindow.h
HEADERS += $$PWD/chatpage.h
HEADERS += $$PWD/connectpage.h
HEADERS += $$PWD/pluginloader.h
HEADERS += $$PWD/settingspage.h
HEADERS += $$PWD/splitview.h
HEADERS += $$PWD/themeinfo.h
HEADERS += $$PWD/themeloader.h

SOURCES += $$PWD/appwindow.cpp
SOURCES += $$PWD/chatpage.cpp
SOURCES += $$PWD/connectpage.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/pluginloader.cpp
SOURCES += $$PWD/settingspage.cpp
SOURCES += $$PWD/splitview.cpp
SOURCES += $$PWD/themeinfo.cpp
SOURCES += $$PWD/themeloader.cpp

include(plugins.pri)
include(../plugins/api/api.pri)
include(../../resources/resources.pri)
include(../../themes/themes.pri)
