######################################################################
# Communi
######################################################################

include(cute/cute.pri)

win32 {
    themes.files = $$SOURCE_ROOT/themes/dark
    themes.path = $$[QT_INSTALL_BINS]/themes
    INSTALLS += themes
} else:mac {
    themes.files = $$SOURCE_ROOT/themes/dark
    themes.path = Contents/Resources/themes
    QMAKE_BUNDLE_DATA += themes
} else:unix {
    themes.files = $$SOURCE_ROOT/themes/dark
    themes.path = /usr/share/themes/communi
    INSTALLS += themes
}
