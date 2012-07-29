DESTDIR= ../../../plugins/docktile
TEMPLATE = lib
TARGET = unitylauncher
DEPENDPATH += .
INCLUDEPATH += . ../../lib/

QT = core dbus gui

CONFIG += plugin \
    link_pkgconfig

PKGCONFIG += dbusmenu-qt

HEADERS += unitylauncher.h
SOURCES += unitylauncher.cpp

