######################################################################
# Communi
######################################################################

RESOURCES += $$PWD/cute/cute.qrc

!build_pass {
    OTHER_FILES += $$files(*.css, true)
    OTHER_FILES += $$files(*.qss, true)
    OTHER_FILES += $$files(*.theme, true)
}

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
