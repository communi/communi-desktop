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

#include "toolbar.h"
#include "helppopup.h"
#include "application.h"
#include "settingsmodel.h"
#include <QLineEdit>
#include <QAction>
#include <QIcon>

ToolBar::ToolBar(QWidget* parent) : QToolBar(parent)
{
    setIconSize(QSize(12, 12));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    d.aboutAction = addAction("", this, SIGNAL(aboutTriggered()));
    d.aboutAction->setToolTip(tr("About"));

    d.helpAction = addAction("", this, SLOT(showHelp()));
    d.helpAction->setToolTip(tr("Help"));

    d.settingsAction = addAction("", this, SIGNAL(settingsTriggered()));
    d.settingsAction->setToolTip(tr("Settings"));

    QWidget* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    addWidget(spacer);

    d.connectAction = addAction("", this, SIGNAL(connectTriggered()));
    d.connectAction->setToolTip(tr("Connect"));

    d.newViewAction = addAction("", this, SIGNAL(joinTriggered()));
    d.newViewAction->setToolTip(tr("Add view"));

    QLineEdit lineEdit;
    lineEdit.setStyleSheet("QLineEdit { border: 1px solid transparent; }");
    setFixedHeight(lineEdit.sizeHint().height());

    connect(Application::settings(), SIGNAL(changed()), this, SLOT(applySettings()));
    applySettings();
}

void ToolBar::showHelp()
{
    HelpPopup* help = new HelpPopup(window());
    help->popup();
}

void ToolBar::applySettings()
{
    SettingsModel* settings = Application::settings();
    QString suffix = settings->value("ui.dark").toBool() ? QString("white") : QString("black");

    d.aboutAction->setIcon(QIcon(QString(":/resources/iconmonstr/about-%1.png").arg(suffix)));
    d.helpAction->setIcon(QIcon(QString(":/resources/iconmonstr/help-%1.png").arg(suffix)));
    d.settingsAction->setIcon(QIcon(QString(":/resources/iconmonstr/settings-%1.png").arg(suffix)));

    d.connectAction->setIcon(QIcon(QString(":/resources/iconmonstr/connect-%1.png").arg(suffix)));
    d.newViewAction->setIcon(QIcon(QString(":/resources/iconmonstr/new-view-%1.png").arg(suffix)));
}
