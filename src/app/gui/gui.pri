######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

FORMS += $$PWD/connectpage.ui

HEADERS += $$PWD/chatpage.h
HEADERS += $$PWD/connectpage.h
HEADERS += $$PWD/mainwindow.h
HEADERS += $$PWD/splitview.h

SOURCES += $$PWD/chatpage.cpp
SOURCES += $$PWD/connectpage.cpp
SOURCES += $$PWD/mainwindow.cpp
SOURCES += $$PWD/splitview.cpp

HEADERS += $$PWD/bufferviewex.h
HEADERS += $$PWD/itemdelegate.h

SOURCES += $$PWD/bufferviewex.cpp
SOURCES += $$PWD/itemdelegate.cpp

HEADERS += $$PWD/treedelegate.h
HEADERS += $$PWD/treewidgetex.h

SOURCES += $$PWD/treedelegate.cpp
SOURCES += $$PWD/treewidgetex.cpp
