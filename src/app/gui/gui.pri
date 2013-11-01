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

include(util/util.pri)
