/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "settingspage.h"
#include "themeloader.h"
#include "themewidget.h"
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
        ui.themeCombo->addItem(theme.name());
    }
    connect(ui.themeCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(setTheme(QString)));
}

SettingsPage::~SettingsPage()
{
}

QString SettingsPage::theme() const
{
    return ui.previewLabel->theme();
}

void SettingsPage::setTheme(const QString& name)
{
    ThemeInfo theme = ThemeLoader::instance()->theme(name);
    ui.previewLabel->setTheme(theme.name());
    ui.authorField->setText(theme.author());
    ui.versionField->setText(theme.version());
    ui.descriptionField->setText(theme.description());
    ui.themeCombo->setCurrentIndex(ui.themeCombo->findText(theme.name()));
}

QString SettingsPage::timeStampFormat() const
{
    return ui.timestampField->text();
}

void SettingsPage::setTimeStampFormat(const QString& format)
{
    ui.timestampField->setText(format);
}

bool SettingsPage::showEvents() const
{
    return ui.eventsBox->isChecked();
}

void SettingsPage::setShowEvents(bool show)
{
    ui.eventsBox->setChecked(show);
}

bool SettingsPage::showDetails() const
{
    return ui.detailsBox->isChecked();
}

void SettingsPage::setShowDetails(bool show)
{
    ui.detailsBox->setChecked(show);
}
