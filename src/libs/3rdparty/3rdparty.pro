######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = 3rdparty

DESTDIR = $$BUILD_ROOT/lib
DEPENDPATH += $$PWD $$PWD/qtdocktile
INCLUDEPATH += $$PWD $$PWD/qtdocktile

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
}

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
