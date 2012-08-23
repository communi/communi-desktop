######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi
!mac:DESTDIR = bin
QT += network

RESOURCES += communi.qrc
win32:RC_FILE = communi.rc
mac:ICON = resources/icons/communi.icns

win32:target.path = $$[QT_INSTALL_BINS]
else:mac:target.path = /Applications
else:target.path = /usr/bin
INSTALLS += target

!mac {
	icon.path = /usr/share/pixmaps
	icon.files = resources/icons/128x128/communi.png
	desktop.path = /usr/share/applications
	desktop.files = resources/communi.desktop
	INSTALLS += icon desktop
}

include(src/src.pri)
include(../examples.pri)
include(../shared/shared.pri)
