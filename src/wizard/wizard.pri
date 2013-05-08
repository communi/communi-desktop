######################################################################
# Communi: wizard.pri
######################################################################

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
QMAKE_CLEAN += $$PWD/*~

FORMS += $$PWD/basicsettingspage.ui
FORMS += $$PWD/advancedsettingspage.ui
FORMS += $$PWD/connectionwizardpage.ui
FORMS += $$PWD/serverwizardpage.ui
FORMS += $$PWD/settingswizardpage.ui
FORMS += $$PWD/userwizardpage.ui

HEADERS += $$PWD/advancedsettingspage.h
HEADERS += $$PWD/basicsettingspage.h
HEADERS += $$PWD/connectionwizard.h
HEADERS += $$PWD/connectionwizardpage.h
HEADERS += $$PWD/serverwizardpage.h
HEADERS += $$PWD/settingswizard.h
HEADERS += $$PWD/settingswizardpage.h
HEADERS += $$PWD/userwizardpage.h

SOURCES += $$PWD/advancedsettingspage.cpp
SOURCES += $$PWD/basicsettingspage.cpp
SOURCES += $$PWD/connectionwizard.cpp
SOURCES += $$PWD/connectionwizardpage.cpp
SOURCES += $$PWD/serverwizardpage.cpp
SOURCES += $$PWD/settingswizard.cpp
SOURCES += $$PWD/settingswizardpage.cpp
SOURCES += $$PWD/userwizardpage.cpp
