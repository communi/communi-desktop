######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += src

lessThan(QT_MAJOR_VERSION, 5): \
    error(Communi requires Qt 5 but Qt $$[QT_VERSION] was detected.)

!exists(src/libs/backend/libcommuni/src/src.pri)|!exists(src/libs/base/shared/shared.pri): \
    error(A Git submodule is missing. Run \'git submodule update --init\' in $${PWD}.)
