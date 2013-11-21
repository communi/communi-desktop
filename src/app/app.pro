######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi
DESTDIR = ../../bin
QT += network

mac:LIBS += -framework SystemConfiguration
else:win32:LIBS += -lole32
else:unix:QT += dbus

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
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
HEADERS += $$PWD/settingspage.h

SOURCES += $$PWD/appwindow.cpp
SOURCES += $$PWD/chatpage.cpp
SOURCES += $$PWD/connectpage.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/settingspage.cpp

include(plugins.pri)
include(../config.pri)
include(../libs/base/base.pri)
include(../libs/util/util.pri)
include(../libs/backend/backend.pri)
include(../libs/3rdparty/3rdparty.pri)
include(../../resources/resources.pri)
