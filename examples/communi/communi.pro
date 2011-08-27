######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi
!mac:DESTDIR = bin
DEPENDPATH += . src
INCLUDEPATH += . src
#CONFIG += silent
DEFINES += VERSION=\"\\\"0.2.0\\\"\"
QT += network script xml
maemo5 {
    QT += dbus
    CONFIG += link_pkgconfig
    PKGCONFIG += mce
}
CONFIG(debug, debug|release) {
    QT += scripttools
}

INCLUDEPATH += ../core/include
LIBS += -L../core/lib -lCore
unix:PRE_TARGETDEPS += ../core/lib/libCore.a

!symbian {
    OBJECTS_DIR = .tmp
    MOC_DIR = .tmp
    RCC_DIR = .tmp
    UI_DIR = .tmp
}

lessThan(QT_MAJOR_VERSION, 4) | lessThan(QT_MINOR_VERSION, 7) {
    error(Communi requires Qt 4.7 or newer but Qt $$[QT_VERSION] was detected.)
}

RESOURCES += communi.qrc
win32:RC_FILE = communi.rc
mac {
    ICON = resources/icons/communi.icns
    LIBS += -L/opt/local/lib -l$$qtLibraryTarget(icuuc) -l$$qtLibraryTarget(icui18n) -l$$qtLibraryTarget(icudata)
}
maemo5 {
    # variables
    isEmpty(PREFIX):PREFIX = /usr/local
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"

    # installs
    INSTALLS += target desktop service iconxpm icon26 icon48 icon64

    target.path = $$BINDIR

    desktop.path = $$DATADIR/applications/hildon
    desktop.files += resources/$${TARGET}.desktop

    service.path = $$DATADIR/dbus-1/services
    #service.files += $${TARGET}.service

    iconxpm.path = $$DATADIR/pixmap
    #iconxpm.files += resources/icons/$${TARGET}.xpm

    icon26.path = $$DATADIR/icons/hicolor/26x26/apps
    icon26.files += resources/icons/26x26/$${TARGET}.png

    icon48.path = $$DATADIR/icons/hicolor/48x48/apps
    icon48.files += resources/icons/48x48/$${TARGET}.png

    icon64.path = $$DATADIR/icons/hicolor/64x64/apps
    icon64.files += resources/icons/64x64/$${TARGET}.png
}
symbian {
    TARGET.UID3 = 0xE55E346F
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    TARGET.CAPABILITY = ReadDeviceData WriteDeviceData
}

include(src/src.pri)
include(translations/translations.pri)
include(../examples.pri)
