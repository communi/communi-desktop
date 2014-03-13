######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD $$PWD/shared
INCLUDEPATH += $$PWD $$PWD/shared

include(../../communi.pri)
communiAddLib(base)
