/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "settingspage.h"
#include "themeloader.h"
#include "themeinfo.h"
#include "chatpage.h"
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
        QLabel* label = new QLabel(ui.content);
        label->setTextFormat(Qt::RichText);
        label->setText(tr("<table><tr>"
                          "<td>"
                          "<img src='data:image/png;base64,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'/>"
                          "</td>"
                          "<td>"
                          "<h1>%1</h1>"
                          "<p><b>Version</b>: %2</p>"
                          "<p><b>Author</b>: %3</p>"
                          "<p>%4</p>"
                          "</td>"
                          "</tr></table>").arg(theme.name(), theme.version(), theme.author(), theme.description()));
        ui.content->layout()->addWidget(label);
    }
}

SettingsPage::~SettingsPage()
{
}

QString SettingsPage::theme() const
{
    //return ui.comboBox->currentText();
    return "Cute";
}
/*
void SettingsPage::setThemes(const QStringList& themes)
{
    //ui.comboBox->clear();
    //ui.comboBox->addItems(themes);
}
*/
