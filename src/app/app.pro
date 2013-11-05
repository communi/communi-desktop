######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi
DESTDIR = ../../bin
QT += network

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
    qtHaveModule(multimedia):QT += multimedia
} else {
    lessThan(QT_MAJOR_VERSION, 4) | lessThan(QT_MINOR_VERSION, 7) {
        error(Communi2/QtWidgets requires Qt 4.7 or newer but Qt $$[QT_VERSION] was detected.)
    }
    CONFIG(phonon, phonon|no_phonon):QT += phonon
}

win32:target.path = $$[QT_INSTALL_BINS]
else:mac:target.path = /Applications
else:target.path = /usr/bin
INSTALLS += target

FORMS += $$PWD/connectpage.ui

HEADERS += $$PWD/chatpage.h
HEADERS += $$PWD/connectpage.h
HEADERS += $$PWD/mainwindow.h

SOURCES += $$PWD/chatpage.cpp
SOURCES += $$PWD/connectpage.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/mainwindow.cpp

include(util/util.pri)
include(../config.pri)
include(../libs/base/base.pri)
include(../libs/util/util.pri)
include(../libs/backend/backend.pri)
include(../libs/3rdparty/3rdparty.pri)
include(../../resources/resources.pri)

STATIC_PLUGINS = badge completer dock finder highlighter history menu monitor navigator sound subject tray
for(PLUGIN, STATIC_PLUGINS) {
    LIBS *= -L$$BUILD_ROOT/plugins
    LIBS += -l$${PLUGIN}plugin
    isEmpty(QMAKE_EXTENSION_STATICLIB):QMAKE_EXTENSION_STATICLIB = lib
    PRE_TARGETDEPS += $$BUILD_ROOT/plugins/$${QMAKE_PREFIX_STATICLIB}$${PLUGIN}plugin.$${QMAKE_EXTENSION_STATICLIB}
}
