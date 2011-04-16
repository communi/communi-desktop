######################################################################
# Communi: src.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

FORMS += $$PWD/messageview.ui

HEADERS += $$PWD/application.h
HEADERS += $$PWD/completer.h
HEADERS += $$PWD/connection.h
HEADERS += $$PWD/maintabwidget.h
HEADERS += $$PWD/mainwindow.h
HEADERS += $$PWD/messageview.h
HEADERS += $$PWD/session.h
HEADERS += $$PWD/sessiontabwidget.h
HEADERS += $$PWD/settings.h
HEADERS += $$PWD/shortcutmanager.h
HEADERS += $$PWD/welcomepage.h

SOURCES += $$PWD/application.cpp
SOURCES += $$PWD/completer.cpp
SOURCES += $$PWD/connection.cpp
SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/maintabwidget.cpp
SOURCES += $$PWD/mainwindow.cpp
SOURCES += $$PWD/messageview.cpp
SOURCES += $$PWD/session.cpp
SOURCES += $$PWD/sessiontabwidget.cpp
SOURCES += $$PWD/settings.cpp
SOURCES += $$PWD/shortcutmanager.cpp
SOURCES += $$PWD/welcomepage.cpp

include(3rdparty/3rdparty.pri)
include(util/util.pri)
include(wizard/wizard.pri)
