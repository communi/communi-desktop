######################################################################
# Communi
######################################################################

STATIC_PLUGINS += ajax
STATIC_PLUGINS += badge
STATIC_PLUGINS += commander
STATIC_PLUGINS += completer
STATIC_PLUGINS += dock
STATIC_PLUGINS += finder
STATIC_PLUGINS += highlighter
STATIC_PLUGINS += history
STATIC_PLUGINS += ignore
STATIC_PLUGINS += lag
STATIC_PLUGINS += menu
STATIC_PLUGINS += monitor
STATIC_PLUGINS += navigator
STATIC_PLUGINS += reseter
STATIC_PLUGINS += sorter
STATIC_PLUGINS += sound
STATIC_PLUGINS += state
STATIC_PLUGINS += style
STATIC_PLUGINS += subject
STATIC_PLUGINS += tray
STATIC_PLUGINS += verifier
STATIC_PLUGINS += znc

for(PLUGIN, STATIC_PLUGINS) {
    LIBS *= -L$$BUILD_ROOT/plugins
    LIBS += -l$${PLUGIN}plugin
    isEmpty(QMAKE_EXTENSION_STATICLIB):QMAKE_EXTENSION_STATICLIB = lib
    PRE_TARGETDEPS += $$BUILD_ROOT/plugins/$${QMAKE_PREFIX_STATICLIB}$${PLUGIN}plugin.$${QMAKE_EXTENSION_STATICLIB}
}
