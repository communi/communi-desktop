######################################################################
# Communi
######################################################################

!verbose:!symbian:CONFIG += silent

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

!symbian {
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
}

macx:!qt_no_framework {
    LIBS += -F$$PWD/../lib -framework Communi
} else {
    TEMPLATE += fakelib
    LIBS += -L$$PWD/../lib -l$$qtLibraryTarget(Communi)
    TEMPLATE -= fakelib
    QMAKE_RPATHDIR += $$PWD/../lib
}
