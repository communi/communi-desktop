######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi
DESTDIR = ../../bin

win32:target.path = $$[QT_INSTALL_BINS]
else:mac:target.path = /Applications
else:target.path = /usr/bin
INSTALLS += target

include(src.pri)
include(util/util.pri)
include(../config.pri)
include(../libs/base/base.pri)
include(../libs/util/util.pri)
include(../libs/backend/backend.pri)
include(../libs/3rdparty/3rdparty.pri)
include(../../resources/resources.pri)

STATIC_PLUGINS = completer finder highlighter history menu navigator subject
for(PLUGIN, STATIC_PLUGINS) {
    LIBS *= -L$$PWD/../../plugins
    LIBS += -l$${PLUGIN}plugin
    PRE_TARGETDEPS += $$PWD/../../plugins/$${QMAKE_PREFIX_STATICLIB}$${PLUGIN}plugin.$${QMAKE_EXTENSION_STATICLIB}
}
