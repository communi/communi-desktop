######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD $$PWD/qtdocktile
INCLUDEPATH += $$PWD $$PWD/qtdocktile

mac: LIBS += -framework AppKit
win32: LIBS += -lole32
unix:!mac: QT += dbus

LIBNAME = 3rdparty
include(../lib.pri)
