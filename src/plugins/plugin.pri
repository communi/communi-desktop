######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += plugin static
TARGET = $$basename(_PRO_FILE_PWD_)plugin

DEPENDPATH += $$_PRO_FILE_PWD_
INCLUDEPATH += $$_PRO_FILE_PWD_

DESTDIR = $$BUILD_ROOT/plugins

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
}

include(../config.pri)
include(../libs/base/base.pri)
include(../libs/backend/backend.pri)
