######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += backend base 3rdparty
base.depends += backend
