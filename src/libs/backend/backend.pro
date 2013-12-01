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

!exists(libcommuni/src/src.pri) {
    error(A Git submodule is missing. Run \'git submodule update --init\' in $${_PRO_FILE_PWD_}.)
}
include(libcommuni/src/src.pri)
