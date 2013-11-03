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
HEADERS += $$PWD/textinputex.h

SOURCES += $$PWD/chatpage.cpp
SOURCES += $$PWD/connectpage.cpp
SOURCES += $$PWD/mainwindow.cpp
SOURCES += $$PWD/splitview.cpp
SOURCES += $$PWD/textinputex.cpp

HEADERS += $$PWD/textbrowserex.h
HEADERS += $$PWD/bufferview.h
HEADERS += $$PWD/itemdelegate.h
HEADERS += $$PWD/topiclabelex.h
HEADERS += $$PWD/listviewex.h

SOURCES += $$PWD/textbrowserex.cpp
SOURCES += $$PWD/bufferview.cpp
SOURCES += $$PWD/itemdelegate.cpp
SOURCES += $$PWD/topiclabelex.cpp
SOURCES += $$PWD/listviewex.cpp

HEADERS += $$PWD/treedelegate.h
HEADERS += $$PWD/treewidgetex.h

SOURCES += $$PWD/treedelegate.cpp
SOURCES += $$PWD/treewidgetex.cpp
