######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = 3rdparty

DESTDIR = $$BUILD_ROOT/lib
DEPENDPATH += $$PWD $$PWD/qtdocktile
INCLUDEPATH += $$PWD $$PWD/qtdocktile

HEADERS += $$PWD/simplecrypt.h
SOURCES += $$PWD/simplecrypt.cpp

HEADERS += $$PWD/qtdocktile/qtdocktile.h
HEADERS += $$PWD/qtdocktile/qtdocktile_p.h

SOURCES += $$PWD/qtdocktile/qtdocktile.cpp

mac {
    LIBS += -framework AppKit
    OBJECTIVE_SOURCES += $$PWD/qtdocktile/qtdocktile_mac.mm
}

unix:!mac {
    QT += dbus
    SOURCES += $$PWD/qtdocktile/qtdocktile_unity.cpp
}

win32 {
    LIBS += -lole32
    HEADERS += $$PWD/qtdocktile/winutils.h
    SOURCES += $$PWD/qtdocktile/qtdocktile_win.cpp
}

include(../../config.pri)
