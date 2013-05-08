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

#include "settingswizard.h"
#include "settingswizardpage.h"

SettingsWizard::SettingsWizard(SettingsModel* settings, QWidget* parent)
    : QWizard(parent), model(settings)
{
    setWindowTitle(tr("Settings"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    SettingsWizardPage* page = new SettingsWizardPage(settings, this);
    setPage(0, page);

    setOption(NoCancelButton, false);
    setOption(NoDefaultButton, false);
    setOption(NoBackButtonOnLastPage, true);
    setOption(NoBackButtonOnStartPage, true);
    setOption(HaveFinishButtonOnEarlyPages, true);

    setOption(HaveCustomButton1, true);
    button(CustomButton1)->setCheckable(true);
    setButtonText(CustomButton1, tr("Advanced"));
    connect(button(CustomButton1), SIGNAL(toggled(bool)), page, SLOT(setAdvancedPage(bool)));

    if (testOption(CancelButtonOnLeft))
        setButtonLayout(QList<WizardButton>() << CustomButton1 << Stretch << CancelButton << FinishButton);
    else
        setButtonLayout(QList<WizardButton>() << CustomButton1 << Stretch << FinishButton << CancelButton);
}
