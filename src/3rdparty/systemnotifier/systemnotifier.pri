######################################################################
# Communi: systemnotifier.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/systemnotifier.h
SOURCES += $$PWD/systemnotifier.cpp

mac:OBJECTIVE_SOURCES += $$PWD/systemnotifier_mac.mm
else:win32:SOURCES += $$PWD/systemnotifier_win.cpp
else:SOURCES += $$PWD/systemnotifier_dbus.cpp

mac {
    LIBS += -framework SystemConfiguration
    HEADERS += $$PWD/Reachability.h
    OBJECTIVE_SOURCES += $$PWD/Reachability.m
} else:unix {
    QT += dbus
} else:win32 {
    HEADERS += $$PWD/win/networknotifier.h
    SOURCES += $$PWD/win/networknotifier.cpp
}
