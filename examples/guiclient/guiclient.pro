######################################################################
# LibIrcClient-Qt
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += libircclient-qt

# Input
HEADERS += connectdialog.h mainwindow.h messageview.h
SOURCES += connectdialog.cpp main.cpp mainwindow.cpp messageview.cpp
FORMS   += connectdialog.ui
