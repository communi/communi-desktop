######################################################################
# LibIrcClient-Qt
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += libircclient-qt

# Input
HEADERS += connectdialog.h mainwindow.h
SOURCES += connectdialog.cpp main.cpp mainwindow.cpp
FORMS   += connectdialog.ui
