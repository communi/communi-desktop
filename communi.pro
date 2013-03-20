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
}
CONFIG += communi
!verbose:CONFIG += silent

lessThan(QT_MAJOR_VERSION, 5) {
    lessThan(QT_MAJOR_VERSION, 4) | lessThan(QT_MINOR_VERSION, 6) {
        error(Communi requires Qt 4.6 or newer but Qt $$[QT_VERSION] was detected.)
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
