######################################################################
# Communi: src.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

SOURCES += $$PWD/main.cpp

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

include(gui/gui.pri)
