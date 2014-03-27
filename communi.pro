######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += src

lessThan(QT_MAJOR_VERSION, 5): \
    error(Communi requires Qt 5 but Qt $$[QT_VERSION] was detected.)

# load(feature, ignore_error)
load(communi-config.prf, true)
!contains(DEFINES, IRC_SHARED): \
    error(The Communi framework is missing. Install libcommuni from http://communi.github.com)

!exists(src/libs/base/shared/shared.pri): \
    error(A Git submodule is missing. Run \'git submodule update --init\' in $${PWD}.)
