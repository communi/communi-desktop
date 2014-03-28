######################################################################
# Communi
######################################################################

TEMPLATE = lib
COMMUNI += core model util
CONFIG += communi_plugin
win:CONFIG += static

HEADERS += $$PWD/commandverifier.h
HEADERS += $$PWD/syntaxhighlighter.h
HEADERS += $$PWD/verifierplugin.h

SOURCES += $$PWD/commandverifier.cpp
SOURCES += $$PWD/syntaxhighlighter.cpp
SOURCES += $$PWD/verifierplugin.cpp
