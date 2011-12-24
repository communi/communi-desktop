######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
contains(MEEGO_EDITION,harmattan):SUBDIRS += meego/communi.pro
else:symbian:SUBDIRS += symbian/communi.pro
else:SUBDIRS += bot desktop/communi.pro
