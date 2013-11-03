######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
LIBS += -L$$PWD/../../../lib -lutil
PRE_TARGETDEPS += $$PWD/../../../lib/$${QMAKE_PREFIX_STATICLIB}util.$${QMAKE_EXTENSION_STATICLIB}
