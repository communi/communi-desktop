load(qt_module)

TARGET = QtDockTile
QT = core network gui

TEMPLATE = lib
DESTDIR = ../../lib

CONFIG += module
MODULE_PRI += ../../modules/qt_docktile.pri

!static:DEFINES += QT_MAKEDLL

load(qt_module_config)

PRIVATE_HEADERS += \
    qtdocktile_p.h \
    pluginloader.h \
    qtdockmanager_p.h

PUBLIC_HEADERS += \
    qtdocktile_global.h \
    qtdocktile.h \
    qtdockprovider.h

SOURCES += \
    pluginloader.cpp \
    qtdocktile.cpp \
    qtdockmanager.cpp \
    qtdockprovider.cpp
    
HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

win32:CONFIG(debug, debug|release):TARGET = $${TARGET}d
