######################################################################
# Communi
######################################################################

TEMPLATE = subdirs
SUBDIRS += filter
SUBDIRS += verifier
SUBDIRS += znc
osx:SUBDIRS += osx
else:win32:SUBDIRS += windows
else:qtHaveModule(x11extras):SUBDIRS += gnome
