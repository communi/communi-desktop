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
else:SOURCES += $$PWD/systemnotifier_dummy.cpp
