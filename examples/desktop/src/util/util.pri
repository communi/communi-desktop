######################################################################
# Communi: util.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/completer.h
HEADERS += $$PWD/historylineedit.h
HEADERS += $$PWD/sharedtimer.h
HEADERS += $$PWD/tabwidget.h
HEADERS += $$PWD/textbrowser.h
HEADERS += $$PWD/trayicon.h

SOURCES += $$PWD/completer.cpp
SOURCES += $$PWD/historylineedit.cpp
SOURCES += $$PWD/sharedtimer.cpp
SOURCES += $$PWD/tabwidget.cpp
SOURCES += $$PWD/textbrowser.cpp
SOURCES += $$PWD/trayicon.cpp
