######################################################################
# Communi: powernotifier.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/powernotifier.h
SOURCES += $$PWD/powernotifier.cpp

mac:OBJECTIVE_SOURCES += $$PWD/powernotifier_mac.mm
else:win32:SOURCES += $$PWD/powernotifier_win.cpp
else:SOURCES += $$PWD/powernotifier_dummy.cpp
