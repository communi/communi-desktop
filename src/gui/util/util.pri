######################################################################
# Communi: util.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/completer.h
HEADERS += $$PWD/editablelabel.h
HEADERS += $$PWD/historylineedit.h
HEADERS += $$PWD/styledscrollbar.h
HEADERS += $$PWD/syntaxhighlighter.h
HEADERS += $$PWD/textbrowser.h

SOURCES += $$PWD/completer.cpp
SOURCES += $$PWD/editablelabel.cpp
SOURCES += $$PWD/historylineedit.cpp
SOURCES += $$PWD/styledscrollbar.cpp
SOURCES += $$PWD/syntaxhighlighter.cpp
SOURCES += $$PWD/textbrowser.cpp
