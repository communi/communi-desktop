######################################################################
# Communi
######################################################################

greaterThan(QT_MAJOR_VERSION, 4) {
    qtHaveModule(multimedia):QT += multimedia
} else {
    CONFIG(phonon, phonon|no_phonon):QT += phonon
}

HEADERS += $$PWD/soundnotification.h
HEADERS += $$PWD/soundplugin.h

SOURCES += $$PWD/soundnotification.cpp
SOURCES += $$PWD/soundplugin.cpp

include(../plugin.pri)
