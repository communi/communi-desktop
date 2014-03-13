######################################################################
# Communi
######################################################################

TEMPLATE = lib
TARGET = backend
CONFIG += static

DESTDIR = $$BUILD_ROOT/lib
DEFINES += IRC_STATIC

!exists(libcommuni/src/src.pri) {
    error(A Git submodule is missing. Run \'git submodule update --init\' in $${SOURCE_ROOT}.)
}
include(libcommuni/src/src.pri)
