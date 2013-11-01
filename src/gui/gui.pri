######################################################################
# Communi: widgets.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

FORMS += $$PWD/connectpage.ui

HEADERS += $$PWD/browser.h
HEADERS += $$PWD/bufferview.h
HEADERS += $$PWD/chatpage.h
HEADERS += $$PWD/completer.h
HEADERS += $$PWD/connectpage.h
HEADERS += $$PWD/document.h
HEADERS += $$PWD/mainwindow.h
HEADERS += $$PWD/searchentry.h
HEADERS += $$PWD/searchpopup.h
HEADERS += $$PWD/splitview.h
HEADERS += $$PWD/textentry.h
HEADERS += $$PWD/topiclabel.h
HEADERS += $$PWD/userlistview.h

SOURCES += $$PWD/browser.cpp
SOURCES += $$PWD/bufferview.cpp
SOURCES += $$PWD/chatpage.cpp
SOURCES += $$PWD/completer.cpp
SOURCES += $$PWD/connectpage.cpp
SOURCES += $$PWD/document.cpp
SOURCES += $$PWD/mainwindow.cpp
SOURCES += $$PWD/searchentry.cpp
SOURCES += $$PWD/searchpopup.cpp
SOURCES += $$PWD/splitview.cpp
SOURCES += $$PWD/textentry.cpp
SOURCES += $$PWD/topiclabel.cpp
SOURCES += $$PWD/userlistview.cpp

include(tree/tree.pri)
include(util/util.pri)
