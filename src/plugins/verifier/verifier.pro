######################################################################
# Communi
######################################################################

TEMPLATE = lib
CONFIG += static communi_plugin

HEADERS += $$PWD/commandverifier.h
HEADERS += $$PWD/syntaxhighlighter.h
HEADERS += $$PWD/verifierplugin.h

SOURCES += $$PWD/commandverifier.cpp
SOURCES += $$PWD/syntaxhighlighter.cpp
SOURCES += $$PWD/verifierplugin.cpp
