######################################################################
# Communi
######################################################################

isEmpty(LIBNAME):error(LIBNAME must be set)
LIBS *= -L$$BUILD_ROOT/lib
LIBS += -l$$LIBNAME
PRE_TARGETDEPS += $$BUILD_ROOT/lib/$${QMAKE_PREFIX_STATICLIB}$${LIBNAME}.$${QMAKE_EXTENSION_STATICLIB}
