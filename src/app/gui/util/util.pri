######################################################################
# Communi: util.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/editablelabel.h
HEADERS += $$PWD/historylineedit.h
HEADERS += $$PWD/styledscrollbar.h
HEADERS += $$PWD/syntaxhighlighter.h

SOURCES += $$PWD/editablelabel.cpp
SOURCES += $$PWD/historylineedit.cpp
SOURCES += $$PWD/styledscrollbar.cpp
SOURCES += $$PWD/syntaxhighlighter.cpp
