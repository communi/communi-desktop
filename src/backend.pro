######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = backend
DESTDIR = ../lib
QT = core network
DEFINES += IRC_STATIC

include(config.pri)
include(backend/src/src.pri)
