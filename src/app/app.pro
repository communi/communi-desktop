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
include(../config.pri)
include(../backend.pri)
include(../util/util.pri)
include(../input/input.pri)
include(../tree/tree.pri)
include(../view/view.pri)
include(../3rdparty/3rdparty.pri)
include(../../resources/resources.pri)

STATIC_PLUGINS = finder highlighter history menu navigator
for(PLUGIN, STATIC_PLUGINS) {
    LIBS *= -L$$PWD/../../plugins
    LIBS += -l$${PLUGIN}plugin
}
