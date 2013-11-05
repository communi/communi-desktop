######################################################################
# Communi
######################################################################

HEADERS += $$PWD/monitorplugin.h
HEADERS += $$PWD/systemmonitor.h

SOURCES += $$PWD/monitorplugin.cpp
SOURCES += $$PWD/systemmonitor.cpp

mac {
    HEADERS += $$PWD/mac/Reachability.h
    OBJECTIVE_SOURCES += $$PWD/mac/Reachability.m
    OBJECTIVE_SOURCES += $$PWD/systemmonitor_mac.mm
    LIBS += -framework SystemConfiguration
} else:win32 {
    HEADERS += $$PWD/win/networkmonitor.h
    SOURCES += $$PWD/win/networkmonitor.cpp
    SOURCES += $$PWD/systemmonitor_win.cpp
    LIBS += -lole32
} else:unix {
    QT += dbus
    SOURCES += $$PWD/systemmonitor_dbus.cpp
} else {
    error(unsupported platform)
}

include(../plugin.pri)
