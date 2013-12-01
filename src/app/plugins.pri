######################################################################
# Communi
######################################################################

STATIC_PLUGINS += commander
STATIC_PLUGINS += dock
STATIC_PLUGINS += finder
STATIC_PLUGINS += ignore
STATIC_PLUGINS += importer
STATIC_PLUGINS += monitor
STATIC_PLUGINS += overlay
STATIC_PLUGINS += sound
STATIC_PLUGINS += title
STATIC_PLUGINS += tray
STATIC_PLUGINS += tricon
STATIC_PLUGINS += verifier
STATIC_PLUGINS += znc

for(PLUGIN, STATIC_PLUGINS) {
    LIBS *= -L$$BUILD_ROOT/plugins
    LIBS += -l$${PLUGIN}plugin
    isEmpty(QMAKE_EXTENSION_STATICLIB):QMAKE_EXTENSION_STATICLIB = lib
    PRE_TARGETDEPS += $$BUILD_ROOT/plugins/$${QMAKE_PREFIX_STATICLIB}$${PLUGIN}plugin.$${QMAKE_EXTENSION_STATICLIB}
}
