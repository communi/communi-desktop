######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static communi_plugin

greaterThan(QT_MAJOR_VERSION, 4) {
    qtHaveModule(multimedia):QT += multimedia
} else {
    CONFIG(phonon, phonon|no_phonon):QT += phonon
}

RESOURCES += alert.qrc

HEADERS += $$PWD/alert.h
HEADERS += $$PWD/alertplugin.h

SOURCES += $$PWD/alert.cpp
SOURCES += $$PWD/alertplugin.cpp
