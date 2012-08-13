######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
contains(MEEGO_EDITION,harmattan)|symbian:SUBDIRS += mobile
else:SUBDIRS += bot desktop
