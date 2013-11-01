######################################################################
# Communi
######################################################################

TEMPLATE = app
win32|mac:TARGET = Communi
else:TARGET = communi
!mac:DESTDIR = bin

win32:target.path = $$[QT_INSTALL_BINS]
else:mac:target.path = /Applications
else:target.path = /usr/bin
INSTALLS += target

include(src.pri)
include(../config.pri)
include(../backend.pri)
include(../util/util.pri)
include(../tree/tree.pri)
include(../../resources/resources.pri)
