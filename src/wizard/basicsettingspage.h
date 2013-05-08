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

#ifndef BASICSETTINGSPAGE_H
#define BASICSETTINGSPAGE_H

#include "ui_basicsettingspage.h"

class SettingsModel;

class BasicSettingsPage : public QWidget
{
    Q_OBJECT

public:
    BasicSettingsPage(QWidget* parent = 0);

    void setSettings(SettingsModel* settings);

private slots:
    void updateUi();
    void updateModel();

private:
    bool block;
    SettingsModel* model;
    Ui::BasicSettingsPage ui;
};

#endif // BASICSETTINGSPAGE_H
