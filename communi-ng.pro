######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += src

QMAKE_CACHE = $${OUT_PWD}$${QMAKE_DIR_SEP}.qmake.cache
system(echo SOURCE_ROOT=$$PWD > $$QMAKE_CACHE)
system(echo BUILD_ROOT=$$OUT_PWD >> $$QMAKE_CACHE)
