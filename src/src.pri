######################################################################
# Communi: src.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

SOURCES += $$PWD/main.cpp

exists(backend/src/src.pri) {
    include(backend/src/src.pri)
} else:load(communi-config, true) {
    # bool load(feature, ignore_error)
    CONFIG += communi
} else {
    error(Cannot detect libcommuni. Either install it or run \'git submodule update --init\' in $${_PRO_FILE_PWD_}.)
}

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
include(util/util.pri)
include(3rdparty/3rdparty.pri)
