######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD $$PWD/qtdocktile
INCLUDEPATH += $$PWD $$PWD/qtdocktile

mac: LIBS += -framework AppKit
win32: LIBS += -lole32
unix:!mac: QT += dbus

include(../../communi.pri)
communiAddLib(3rdparty)
