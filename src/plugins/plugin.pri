######################################################################
# Communi
######################################################################

TEMPLATE = lib
TARGET = $$basename(_PRO_FILE_PWD_)plugin
CONFIG += plugin static communi_config communi_backend communi_base

DESTDIR = $$BUILD_ROOT/plugins
DEPENDPATH += $$_PRO_FILE_PWD_
INCLUDEPATH += $$_PRO_FILE_PWD_

include(api/api.pri)
