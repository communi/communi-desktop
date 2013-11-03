######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += $$PWD/systemnotifier.h
SOURCES += $$PWD/systemnotifier.cpp

mac {
    HEADERS += $$PWD/mac/Reachability.h
    OBJECTIVE_SOURCES += $$PWD/mac/Reachability.m
    OBJECTIVE_SOURCES += $$PWD/systemnotifier_mac.mm
    LIBS += -framework SystemConfiguration
} else:win32 {
    HEADERS += $$PWD/win/networknotifier.h
    SOURCES += $$PWD/win/networknotifier.cpp
    SOURCES += $$PWD/systemnotifier_win.cpp
    LIBS += -lole32
} else:unix {
    QT += dbus
    SOURCES += $$PWD/systemnotifier_dbus.cpp
} else {
    error(unsupported platform)
}
