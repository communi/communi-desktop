######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
contains(MEEGO_EDITION,harmattan):SUBDIRS += meego/communi.pro
else:SUBDIRS += bot desktop/communi.pro
