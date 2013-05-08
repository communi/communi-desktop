/*
* Copyright (C) 2008-2013 The Communi Project
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "settingswizardpage.h"

SettingsWizardPage::SettingsWizardPage(SettingsModel* settings, QWidget* parent)
    : QWizardPage(parent), settings(settings)
{
    ui.setupUi(this);
    ui.basicPage->setSettings(settings);
    ui.advancedPage->setSettings(settings);

    setFinalPage(true);
    setAdvancedPage(false);
}

void SettingsWizardPage::setAdvancedPage(bool advanced)
{
    if (advanced) {
        setTitle(tr("Advanced settings"));
        setSubTitle(tr("Adjust advanced settings"));
        ui.stackedWidget->setCurrentWidget(ui.advancedPage);
    } else {
        setTitle(tr("Basic settings"));
        setSubTitle(tr("Adjust basic settings"));
        ui.stackedWidget->setCurrentWidget(ui.basicPage);
    }
}
