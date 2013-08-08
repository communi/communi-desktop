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

#include "basicsettingspage.h"
#include "settingsmodel.h"

BasicSettingsPage::BasicSettingsPage(QWidget* parent)
    : QWidget(parent), block(false), model(0)
{
    ui.setupUi(this);

    connect(ui.themeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(updateModel()));
    connect(ui.fontComboBox, SIGNAL(currentFontChanged(QFont)), SLOT(updateModel()));
    connect(ui.sizeSpinBox, SIGNAL(valueChanged(int)), SLOT(updateModel()));
    connect(ui.blockSpinBox, SIGNAL(valueChanged(int)), SLOT(updateModel()));
    connect(ui.timeStampEdit, SIGNAL(textEdited(QString)), SLOT(updateModel()));
    connect(ui.stripNicksCheckBox, SIGNAL(toggled(bool)), SLOT(updateModel()));
    connect(ui.hideEventsCheckBox, SIGNAL(toggled(bool)), SLOT(updateModel()));
}

void BasicSettingsPage::setSettings(SettingsModel* settings)
{
    model = settings;
    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(updateUi()));
    updateUi();
}

void BasicSettingsPage::updateUi()
{
    if (!block) {
        block = true;
        ui.themeComboBox->setCurrentIndex(model->value("ui.dark").toBool() ? 1 : 0);
        QFont font = model->value("ui.font").value<QFont>();
        ui.fontComboBox->setCurrentFont(font);
        ui.sizeSpinBox->setValue(font.pointSize());
        ui.blockSpinBox->setValue(model->value("ui.scrollback").toInt());
        ui.stripNicksCheckBox->setChecked(model->value("formatting.hideUserHosts").toBool());
        ui.timeStampEdit->setText(model->value("formatting.timeStamp").toString());
        ui.hideEventsCheckBox->setChecked(!model->value("messages.joins").toBool() &&
                                          !model->value("messages.parts").toBool() &&
                                          !model->value("messages.quits").toBool());
        block = false;
    }
}

void BasicSettingsPage::updateModel()
{
    if (!block) {
        block = true;
        model->setValue("ui.dark", ui.themeComboBox->currentIndex() != 0);
        QFont font = ui.fontComboBox->currentFont();
        font.setPointSize(ui.sizeSpinBox->value());
        model->setValue("ui.font", font);
        model->setValue("ui.scrollback", ui.blockSpinBox->value());
        model->setValue("formatting.hideUserHosts", ui.stripNicksCheckBox->isChecked());
        model->setValue("formatting.timeStamp", ui.timeStampEdit->text());
        model->setValue("messages.joins", !ui.hideEventsCheckBox->isChecked());
        model->setValue("messages.parts", !ui.hideEventsCheckBox->isChecked());
        model->setValue("messages.quits", !ui.hideEventsCheckBox->isChecked());
        block = false;
    }
}
