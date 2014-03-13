######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = backend
DESTDIR = $$BUILD_ROOT/lib
QT = core network
DEFINES += IRC_STATIC

include(../../communi.pri)

!exists(libcommuni/src/src.pri) {
    error(A Git submodule is missing. Run \'git submodule update --init\' in $${SOURCE_ROOT}.)
}
include(libcommuni/src/src.pri)
