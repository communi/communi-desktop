######################################################################
# Communi: qtdocktile/windows.pri
######################################################################

LIBS += -lole32

HEADERS += $$PWD/winutils.h
HEADERS += $$PWD/taskbar.h
SOURCES += $$PWD/taskbar.cpp
SOURCES += $$PWD/qtdocktile_win.cpp
