######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += src

lessThan(QT_MAJOR_VERSION, 5): \
    error(Communi requires Qt 5 but Qt $$[QT_VERSION] was detected.)

QMAKE_CONF = $${OUT_PWD}$${QMAKE_DIR_SEP}.qmake.conf
system(echo CONFIG+=communi_config > $$QMAKE_CONF)
system(echo SOURCE_ROOT=$$PWD >> $$QMAKE_CONF)
system(echo BUILD_ROOT=$$OUT_PWD >> $$QMAKE_CONF)
