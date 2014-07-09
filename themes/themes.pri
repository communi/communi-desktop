######################################################################
# Communi
######################################################################

RESOURCES += $$PWD/cute/cute.qrc

!build_pass {
    OTHER_FILES += $$files(*.css, true)
    OTHER_FILES += $$files(*.qss, true)
    OTHER_FILES += $$files(*.theme, true)
}

load(communi_installs.prf)
isEmpty(COMMUNI_INSTALL_THEMES):error(COMMUNI_INSTALL_THEMES empty!)

mac {
    themes.files = $$SOURCE_TREE/themes/dark
    themes.path = Contents/Resources/themes
    QMAKE_BUNDLE_DATA += themes
} else {
    themes.files = $$SOURCE_TREE/themes/dark
    themes.path = $$COMMUNI_INSTALL_THEMES
    INSTALLS += themes
}
