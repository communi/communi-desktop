######################################################################
# Communi
######################################################################

TEMPLATE = lib
TARGET = backend
CONFIG += static

DESTDIR = $$BUILD_TREE/lib
DEFINES += IRC_STATIC

!exists(libcommuni/src/src.pri) {
    error(A Git submodule is missing. Run \'git submodule update --init\' in $${SOURCE_TREE}.)
}
include(libcommuni/src/src.pri)
