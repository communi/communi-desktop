######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
LIBS += -L$$PWD/../../lib -linput
PRE_TARGETDEPS += $$PWD/../../lib/$${QMAKE_PREFIX_STATICLIB}input.$${QMAKE_EXTENSION_STATICLIB}
