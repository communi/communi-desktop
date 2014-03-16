/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "settingspage.h"
#include "themewidget.h"
#include "themeloader.h"
#include <QPushButton>
#include <QShortcut>

SettingsPage::SettingsPage(QWidget* parent) : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.buttonBox, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SIGNAL(rejected()));

    QShortcut* shortcut = new QShortcut(Qt::Key_Return, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Ok), SLOT(click()));

    shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Cancel), SLOT(click()));

    foreach (const QString& name, ThemeLoader::instance()->themes()) {
        ThemeInfo theme = ThemeLoader::instance()->theme(name);
        ThemeWidget* widget = new ThemeWidget(theme, ui.content);
        connect(widget, SIGNAL(selected(ThemeInfo)), this, SLOT(select(ThemeInfo)));
        ui.contentLayout->addWidget(widget);
        ui.widgets += widget;
    }
}

SettingsPage::~SettingsPage()
{
}

QString SettingsPage::theme() const
{
    return ui.theme;
}

void SettingsPage::setTheme(const QString& theme)
{
    select(ThemeLoader::instance()->theme(theme));
}

void SettingsPage::select(const ThemeInfo& theme)
{
    ui.theme = theme.name();
    foreach (ThemeWidget* widget, ui.widgets)
        widget->setSelected(widget->theme().name() == theme.name());
}
