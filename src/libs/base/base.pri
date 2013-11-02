######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
LIBS += -L$$PWD/../../../lib -lbase
PRE_TARGETDEPS += $$PWD/../../../lib/$${QMAKE_PREFIX_STATICLIB}base.$${QMAKE_EXTENSION_STATICLIB}
