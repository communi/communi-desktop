######################################################################
# Communi: settings.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/settingsdelegate.h
HEADERS += $$PWD/settingsmodel.h
HEADERS += $$PWD/settingsview.h

SOURCES += $$PWD/settingsdelegate.cpp
SOURCES += $$PWD/settingsmodel.cpp
SOURCES += $$PWD/settingsview.cpp
