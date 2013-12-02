######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static
TARGET = util

DESTDIR = $$BUILD_ROOT/lib

include(../../config.pri)
include(../backend/backend.pri)

!exists(shared/shared.pri) {
    error(A Git submodule is missing. Run \'git submodule update --init\' in $${SOURCE_ROOT}.)
}
include(shared/shared.pri)
