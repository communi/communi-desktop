######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = 3rdparty

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
    qtHaveModule(multimedia):QT += multimedia
} else {
    lessThan(QT_MAJOR_VERSION, 4) | lessThan(QT_MINOR_VERSION, 7) {
        error(Communi2/QtWidgets requires Qt 4.7 or newer but Qt $$[QT_VERSION] was detected.)
    }
    CONFIG(phonon, phonon|no_phonon):QT += phonon
}

DESTDIR = ../../lib
DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

DEPENDPATH += $$PWD $$PWD/qtdocktile
INCLUDEPATH += $$PWD $$PWD/qtdocktile

HEADERS += $$PWD/simplecrypt.h
SOURCES += $$PWD/simplecrypt.cpp

HEADERS += $$PWD/fancylineedit.h
SOURCES += $$PWD/fancylineedit.cpp

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

include(../config.pri)
