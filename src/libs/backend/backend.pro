######################################################################
# Communi
######################################################################

TEMPLATE = lib
TARGET = backend
CONFIG += static

DESTDIR = $$BUILD_TREE/lib
DEFINES += IRC_STATIC

include(libcommuni/src/src.pri)
