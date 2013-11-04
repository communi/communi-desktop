######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = backend
DESTDIR = $$BUILD_ROOT/lib
QT = core network
DEFINES += IRC_STATIC

include(../../config.pri)
include(libcommuni/src/src.pri)
