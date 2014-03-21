######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi
CONFIG += communi_3rdparty communi_backend communi_base communi_plugins

DESTDIR = ../../bin
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

mac:LIBS += -framework SystemConfiguration
else:win32:LIBS += -lole32
else:unix:QT += dbus
qtHaveModule(multimedia):QT += multimedia

win32:target.path = $$[QT_INSTALL_BINS]
else:mac:target.path = /Applications
else:target.path = /usr/bin
INSTALLS += target

RESOURCES += app.qrc
RESOURCES += ../../communi.qrc
win32:RC_FILE = ../../communi.rc
mac:ICON = ../../images/communi.icns

unix:!mac {
    ICONS_SIZES = 16 24 32 48 64 128
    for(size, ICONS_SIZES) {
        eval(icon$${size}.files = ../../images/$${size}x$${size}/communi.png)
        eval(icon$${size}.path = /usr/share/icons/hicolor/$${size}x$${size}/apps)
        INSTALLS += icon$${size}
    }
    icon.path = /usr/share/pixmaps
    icon.files = ../../images/128x128/communi.png
    desktop.path = /usr/share/applications
    desktop.files = ../../communi.desktop
    INSTALLS += icon desktop
}

OTHER_FILES += ../../communi.rc
OTHER_FILES += ../../communi.desktop

FORMS += $$PWD/connectpage.ui
FORMS += $$PWD/settingspage.ui

HEADERS += $$PWD/alert.h
HEADERS += $$PWD/chatpage.h
HEADERS += $$PWD/connectpage.h
HEADERS += $$PWD/dock.h
HEADERS += $$PWD/helppopup.h
HEADERS += $$PWD/mainwindow.h
HEADERS += $$PWD/pluginloader.h
HEADERS += $$PWD/settingspage.h
HEADERS += $$PWD/splitview.h
HEADERS += $$PWD/overlay.h

SOURCES += $$PWD/alert.cpp
SOURCES += $$PWD/chatpage.cpp
SOURCES += $$PWD/connectpage.cpp
SOURCES += $$PWD/dock.cpp
SOURCES += $$PWD/helppopup.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/mainwindow.cpp
SOURCES += $$PWD/pluginloader.cpp
SOURCES += $$PWD/settingspage.cpp
SOURCES += $$PWD/splitview.cpp
SOURCES += $$PWD/overlay.cpp

include(finder/finder.pri)
include(theme/theme.pri)
include(tree/tree.pri)
include(../../themes/themes.pri)
