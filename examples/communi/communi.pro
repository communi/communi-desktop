######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi
!mac:DESTDIR = bin
QT += network
maemo5 {
    QT += dbus
    CONFIG += link_pkgconfig
    PKGCONFIG += mce
}

RESOURCES += communi.qrc
win32:RC_FILE = communi.rc
mac:ICON = resources/icons/communi.icns
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

include(src/src.pri)
include(../examples.pri)
include(../shared/shared.pri)
