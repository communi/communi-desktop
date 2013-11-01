######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD $$PWD/systemnotifier
INCLUDEPATH += $$PWD $$PWD/systemnotifier
LIBS += -L$$PWD/../../lib -lutil
PRE_TARGETDEPS += $$PWD/../../lib/$${QMAKE_PREFIX_STATICLIB}util.$${QMAKE_EXTENSION_STATICLIB}
