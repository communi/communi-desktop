anna|belle|symbian {
    qml_files.source = qml/symbian
    DEFINES += COMMUNI_PLATFORM=\\\"Symbian\\\"
    DEFINES += COMMUNI_EXAMPLE_VERSION=\\\"1.1.1\\\"
    DEFINES += COMMUNI_MAIN_QML=\\\"qml/symbian/main.qml\\\"
} else {
    qml_files.source = qml/meego
    DEFINES += COMMUNI_PLATFORM=\\\"MeeGo\\\"
    DEFINES += COMMUNI_EXAMPLE_VERSION=\\\"1.1.8\\\"
    DEFINES += COMMUNI_MAIN_QML=\\\"qml/meego/main.qml\\\"
    DEFINES += COMMUNI_IMPORT_PATH=\\\"/opt/communi/imports\\\"
}

# Add more folders to ship with the application, here
qml_files.target = qml
qml_images.source = qml/images
qml_images.target = qml
DEPLOYMENTFOLDERS = qml_files qml_images

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

symbian {
    TARGET = Communi
    TARGET.UID3 = 0x20035734
    # Allow network access on Symbian
    TARGET.CAPABILITY += NetworkServices
}

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
# CONFIG += qdeclarative-boostable

# Add dependency to Symbian components
CONFIG += qt-components

INCLUDEPATH += src
DEPENDPATH += src

# Sources
HEADERS += abstractsessionitem.h
HEADERS += completer.h
HEADERS += sessionchilditem.h
HEADERS += sessionitem.h
HEADERS += sessionmanager.h
HEADERS += settings.h

SOURCES += abstractsessionitem.cpp
SOURCES += completer.cpp
SOURCES += main.cpp
SOURCES += sessionchilditem.cpp
SOURCES += sessionitem.cpp
SOURCES += sessionmanager.cpp
SOURCES += settings.cpp

# Communi
QT += network
include(../examples.pri)
include(../shared/shared.pri)
QMAKE_RPATHDIR += /opt/communi/lib

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog


