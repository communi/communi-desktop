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

RESOURCES += ../../communi.qrc
win32:RC_FILE = ../../communi.rc
mac:ICON = ../../images/communi.icns
mac:QMAKE_INFO_PLIST = ../../communi.plist

unix:!mac {
    icon-s.files = ../../images/communi-s.png
    icon-s.path = /usr/share/icons/hicolor/64x64/apps/communi.png
    icon-m.files = ../../images/communi-m.png
    icon-m.path = /usr/share/icons/hicolor/256x256/apps/communi.png
    icon-l.files = ../../images/communi-l.png
    icon-l.path = /usr/share/icons/hicolor/512x512/apps/communi.png
    desktop.path = /usr/share/applications
    desktop.files = ../../communi.desktop
    INSTALLS += icon-s icon-m icon-l desktop
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
HEADERS += $$PWD/settingspage.h
HEADERS += $$PWD/splitview.h
HEADERS += $$PWD/overlay.h

SOURCES += $$PWD/chatpage.cpp
SOURCES += $$PWD/connectpage.cpp
SOURCES += $$PWD/helppopup.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/mainwindow.cpp
SOURCES += $$PWD/pluginloader.cpp
SOURCES += $$PWD/settingspage.cpp
SOURCES += $$PWD/splitview.cpp
SOURCES += $$PWD/overlay.cpp

include(dock/dock.pri)
include(finder/finder.pri)
include(theme/theme.pri)
include(tree/tree.pri)
include(../../themes/themes.pri)
