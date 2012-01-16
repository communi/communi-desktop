######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
contains(MEEGO_EDITION,harmattan)|symbian:SUBDIRS += mobile/communi.pro
else:SUBDIRS += bot desktop/communi.pro
