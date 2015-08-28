/*
  Copyright (C) 2008-2015 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "settingspage.h"
#include "scrollbarstyle.h"
#include "themeloader.h"
#include "themewidget.h"
#include "themeinfo.h"
#include <QPushButton>
#include <QScrollBar>
#include <QShortcut>

SettingsPage::SettingsPage(QWidget* parent) : QWidget(parent)
{
    ui.setupUi(this);
    setFocusProxy(ui.themeCombo);

    ui.scrollArea->horizontalScrollBar()->setStyle(ScrollBarStyle::narrow());
    ui.scrollArea->verticalScrollBar()->setStyle(ScrollBarStyle::narrow());

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
