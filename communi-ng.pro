######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += src

lessThan(QT_MAJOR_VERSION, 5): \
    error(Communi requires Qt 5 but Qt $$[QT_VERSION] was detected.)

QMAKE_CACHE = $${OUT_PWD}$${QMAKE_DIR_SEP}.qmake.cache
system(echo SOURCE_ROOT=$$PWD > $$QMAKE_CACHE)
system(echo BUILD_ROOT=$$OUT_PWD >> $$QMAKE_CACHE)
