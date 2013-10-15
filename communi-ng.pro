######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi
!mac:DESTDIR = bin
!verbose:CONFIG += silent

win32:target.path = $$[QT_INSTALL_BINS]
else:mac:target.path = /Applications
else:target.path = /usr/bin
INSTALLS += target

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
include(resources/resources.pri)
