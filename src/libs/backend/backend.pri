######################################################################
# Communi
######################################################################

DEFINES += IRC_STATIC
INCDIR = $$PWD/libcommuni/include
DEPENDPATH += $$INCDIR/IrcCore $$INCDIR/IrcModel $$INCDIR/IrcUtil
INCLUDEPATH += $$INCDIR/IrcCore $$INCDIR/IrcModel $$INCDIR/IrcUtil
LIBS += -L$$PWD/../../../lib -lbackend
PRE_TARGETDEPS += $$PWD/../../../lib/$${QMAKE_PREFIX_STATICLIB}backend.$${QMAKE_EXTENSION_STATICLIB}
