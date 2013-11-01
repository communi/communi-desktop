######################################################################
# Communi
######################################################################

DEFINES += IRC_STATIC
INCDIR = $$PWD/backend/include
DEPENDPATH += $$INCDIR/IrcCore $$INCDIR/IrcModel $$INCDIR/IrcUtil
INCLUDEPATH += $$INCDIR/IrcCore $$INCDIR/IrcModel $$INCDIR/IrcUtil
LIBS += -L../../lib -lbackend
