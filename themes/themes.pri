######################################################################
# Communi
######################################################################

include(cute/cute.pri)

mac {
    themes.files = $$SOURCE_ROOT/themes/dark
    themes.path = Contents/Resources/themes
    QMAKE_BUNDLE_DATA += themes
}

win32 {
    themes.files = $$SOURCE_ROOT/themes/dark
    themes.path = $$[QT_INSTALL_BINS]/themes
    INSTALLS += themes
}
