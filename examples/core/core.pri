######################################################################
# Communi
######################################################################

DEPENDPATH += ../core/include
INCLUDEPATH += ../core/include
LIBS += -L../core/lib -lCore
unix:PRE_TARGETDEPS += ../core/lib/libCore.a
