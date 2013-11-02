######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += backend util base 3rdparty
util.depends += backend
base.depends += util
