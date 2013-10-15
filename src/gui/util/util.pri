######################################################################
# Communi: util.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

HEADERS += $$PWD/completer.h
HEADERS += $$PWD/editablelabel.h
HEADERS += $$PWD/lineeditor.h
HEADERS += $$PWD/textbrowser.h
HEADERS += $$PWD/textdocument.h

SOURCES += $$PWD/completer.cpp
SOURCES += $$PWD/editablelabel.cpp
SOURCES += $$PWD/lineeditor.cpp
SOURCES += $$PWD/textbrowser.cpp
SOURCES += $$PWD/textdocument.cpp
