######################################################################
# Communi
######################################################################

TEMPLATE = lib
COMMUNI += core
CONFIG += communi_plugin

HEADERS += $$PWD/monitorplugin.h
HEADERS += $$PWD/systemmonitor.h

SOURCES += $$PWD/monitorplugin.cpp
SOURCES += $$PWD/systemmonitor.cpp

mac {
    HEADERS += $$PWD/mac/Reachability.h
    OBJECTIVE_SOURCES += $$PWD/mac/Reachability.m
    OBJECTIVE_SOURCES += $$PWD/systemmonitor_mac.mm
    LIBS += -framework SystemConfiguration -framework AppKit
} else:win32 {
    DEFINES += _WIN32_WINNT=0x0600
    HEADERS += $$PWD/win/netlistmgr_util.h
    HEADERS += $$PWD/win/networkmonitor.h
    SOURCES += $$PWD/win/networkmonitor.cpp
    SOURCES += $$PWD/systemmonitor_win.cpp
    LIBS += -lole32 -luuid
} else:unix {
    QT += dbus
    SOURCES += $$PWD/systemmonitor_dbus.cpp
} else {
    error(unsupported platform)
}
