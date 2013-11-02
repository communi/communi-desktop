######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += backend base 3rdparty util
util.depends += backend
