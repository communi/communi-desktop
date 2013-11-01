######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
LIBS += -L$$PWD/../../lib -lview
PRE_TARGETDEPS += $$PWD/../../lib/$${QMAKE_PREFIX_STATICLIB}view.$${QMAKE_EXTENSION_STATICLIB}
