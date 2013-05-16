######################################################################
# Communi
######################################################################

TEMPLATE = app
QMAKE_PROJECT_NAME = desktop
win32|mac:TARGET = Communi
else:TARGET = communi
!mac:DESTDIR = bin
QT += network
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
    qtHaveModule(multimedia):QT += multimedia
} else:CONFIG(phonon, phonon|no_phonon) {
    QT += phonon
}
!verbose:CONFIG += silent

lessThan(QT_MAJOR_VERSION, 5) {
    lessThan(QT_MAJOR_VERSION, 4) | lessThan(QT_MINOR_VERSION, 7) {
        error(Communi requires Qt 4.7 or newer but Qt $$[QT_VERSION] was detected.)
    }
}

RESOURCES += communi.qrc
win32:RC_FILE = communi.rc
mac:ICON = resources/icons/communi.icns

win32:target.path = $$[QT_INSTALL_BINS]
else:mac:target.path = /Applications
else:target.path = /usr/bin
INSTALLS += target

unix:!mac {
    ICONS_SIZES = 16 24 32 48 64 128
    for(size, ICONS_SIZES) {
        eval(icon$${size}.files = resources/icons/$${size}x$${size}/communi.png)
        eval(icon$${size}.path = /usr/share/icons/hicolor/$${size}x$${size}/apps)
        INSTALLS += icon$${size}
    }
    icon.path = /usr/share/pixmaps
    icon.files = resources/icons/128x128/communi.png
    desktop.path = /usr/share/applications
    desktop.files = resources/communi.desktop
    INSTALLS += icon desktop
}

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

include(src/src.pri)
