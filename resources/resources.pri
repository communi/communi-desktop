######################################################################
# Communi: resources.pri
######################################################################

RESOURCES += $$PWD/communi.qrc
win32:RC_FILE = $$PWD/communi.rc
mac:ICON = $$PWD/icons/communi.icns

unix:!mac {
    ICONS_SIZES = 16 24 32 48 64 128
    for(size, ICONS_SIZES) {
        eval(icon$${size}.files = $$PWD/icons/$${size}x$${size}/communi.png)
        eval(icon$${size}.path = /usr/share/icons/hicolor/$${size}x$${size}/apps)
        INSTALLS += icon$${size}
    }
    icon.path = /usr/share/pixmaps
    icon.files = $$PWD/icons/128x128/communi.png
    desktop.path = /usr/share/applications
    desktop.files = $$PWD/communi.desktop
    INSTALLS += icon desktop
}

OTHER_FILES += $$PWD/communi.desktop
OTHER_FILES += $$PWD/communi.rc
OTHER_FILES += $$PWD/stylesheet.css
