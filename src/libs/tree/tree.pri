######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
LIBS += -L$$PWD/../../../lib -ltree
PRE_TARGETDEPS += $$PWD/../../../lib/$${QMAKE_PREFIX_STATICLIB}tree.$${QMAKE_EXTENSION_STATICLIB}
