######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += api backend 3rdparty util
util.depends += backend
