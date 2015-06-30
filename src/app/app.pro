######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi

CONFIG += communi
COMMUNI += core model util
CONFIG += communi_base

DESTDIR = ../../bin
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

mac:LIBS += -framework AppKit -framework SystemConfiguration
else:win32:LIBS += -lole32 -luuid
else:unix:QT += dbus
qtHaveModule(multimedia):QT += multimedia

load(communi_installs.prf)
isEmpty(COMMUNI_INSTALL_BINS):error(COMMUNI_INSTALL_BINS empty!)
isEmpty(COMMUNI_INSTALL_PLUGINS):error(COMMUNI_INSTALL_PLUGINS empty!)
isEmpty(COMMUNI_INSTALL_ICONS):error(COMMUNI_INSTALL_ICONS empty!)
isEmpty(COMMUNI_INSTALL_DESKTOP):error(COMMUNI_INSTALL_DESKTOP empty!)
isEmpty(COMMUNI_INSTALL_THEMES):error(COMMUNI_INSTALL_THEMES empty!)

DEFINES += COMMUNI_INSTALL_PLUGINS=\\\"$$COMMUNI_INSTALL_PLUGINS\\\"
DEFINES += COMMUNI_INSTALL_THEMES=\\\"$$COMMUNI_INSTALL_THEMES\\\"

target.path = $$COMMUNI_INSTALL_BINS
INSTALLS += target

RESOURCES += ../../communi.qrc
win32:RC_FILE = ../../communi.rc

mac {
    ICON = ../../images/communi.icns
    QMAKE_INFO_PLIST = ../../communi.plist

    plugins.files += $$BUILD_TREE/plugins/libawayplugin.$$QMAKE_EXTENSION_SHLIB
    plugins.files += $$BUILD_TREE/plugins/libfilterplugin.$$QMAKE_EXTENSION_SHLIB
    plugins.files += $$BUILD_TREE/plugins/libosxplugin.$$QMAKE_EXTENSION_SHLIB
    plugins.files += $$BUILD_TREE/plugins/libverifierplugin.$$QMAKE_EXTENSION_SHLIB
    plugins.files += $$BUILD_TREE/plugins/libzncplugin.$$QMAKE_EXTENSION_SHLIB
    plugins.path = Contents/PlugIns
    QMAKE_BUNDLE_DATA += plugins
}

unix:!mac {
    ICONS_SIZES = 16 24 32 48 64 128 256 512
    for(size, ICONS_SIZES) {
        eval(icon$${size}.files = ../../images/icons/$${size}x$${size}/apps/communi.png)
        eval(icon$${size}.path = $$COMMUNI_INSTALL_ICONS/$${size}x$${size}/apps)
        INSTALLS += icon$${size}
    }
    desktop.files = ../../communi.desktop
    desktop.path = $$COMMUNI_INSTALL_DESKTOP
    INSTALLS += desktop
}

OTHER_FILES += ../../communi.rc
OTHER_FILES += ../../communi.desktop

FORMS += $$PWD/connectpage.ui
FORMS += $$PWD/settingspage.ui

HEADERS += $$PWD/chatpage.h
HEADERS += $$PWD/connectpage.h
HEADERS += $$PWD/helppopup.h
HEADERS += $$PWD/mainwindow.h
HEADERS += $$PWD/pluginloader.h
HEADERS += $$PWD/scrollbarstyle.h
HEADERS += $$PWD/settingspage.h
HEADERS += $$PWD/splitview.h
HEADERS += $$PWD/overlay.h

SOURCES += $$PWD/chatpage.cpp
SOURCES += $$PWD/connectpage.cpp
SOURCES += $$PWD/helppopup.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/mainwindow.cpp
SOURCES += $$PWD/pluginloader.cpp
SOURCES += $$PWD/scrollbarstyle.cpp
SOURCES += $$PWD/settingspage.cpp
SOURCES += $$PWD/splitview.cpp
SOURCES += $$PWD/overlay.cpp

include(3rdparty/3rdparty.pri)
include(dock/dock.pri)
include(finder/finder.pri)
include(monitor/monitor.pri)
include(theme/theme.pri)
include(tree/tree.pri)
include(../../themes/themes.pri)
