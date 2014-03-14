######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += src

lessThan(QT_MAJOR_VERSION, 5): \
    error(Communi requires Qt 5 but Qt $$[QT_VERSION] was detected.)
