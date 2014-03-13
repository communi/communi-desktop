######################################################################
# Communi
######################################################################

!verbose:CONFIG += silent

CONFIG(debug, debug|release) {
    OBJECTS_DIR = debug
    MOC_DIR = debug
    RCC_DIR = debug
    UI_DIR = debug
} else {
    OBJECTS_DIR = release
    MOC_DIR = release
    RCC_DIR = release
    UI_DIR = release
}

isEmpty(QMAKE_EXTENSION_STATICLIB):QMAKE_EXTENSION_STATICLIB = lib

defineTest(communiAddLib) {
    LIBNAME = $$1
    isEmpty(LIBNAME):error(LIBNAME must be set)
    LIBS *= -L$$BUILD_ROOT/lib
    LIBS += -l$$LIBNAME
    PRE_TARGETDEPS += $$BUILD_ROOT/lib/$${QMAKE_PREFIX_STATICLIB}$${LIBNAME}.$${QMAKE_EXTENSION_STATICLIB}
    export(LIBS)
    export(PRE_TARGETDEPS)
}
