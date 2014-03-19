######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static communi_plugin
qtHaveModule(multimedia):QT += multimedia

RESOURCES += alert.qrc

HEADERS += $$PWD/alert.h
HEADERS += $$PWD/alertplugin.h

SOURCES += $$PWD/alert.cpp
SOURCES += $$PWD/alertplugin.cpp
