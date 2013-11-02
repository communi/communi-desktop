######################################################################
# Communi
######################################################################

DEPENDPATH += $$PWD $$PWD/qtdocktile
INCLUDEPATH += $$PWD $$PWD/qtdocktile
LIBS += -L$$PWD/../../../lib -l3rdparty
PRE_TARGETDEPS += $$PWD/../../../lib/$${QMAKE_PREFIX_STATICLIB}3rdparty.$${QMAKE_EXTENSION_STATICLIB}

mac: LIBS += -framework AppKit
win32: LIBS += -lole32
unix:!mac: QT += dbus
