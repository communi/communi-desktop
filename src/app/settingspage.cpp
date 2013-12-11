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
#include <QPainter>
#include <QPixmap>
#include <QBuffer>

SettingsPage::SettingsPage(QWidget* parent) : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.buttonBox, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SIGNAL(rejected()));

    QShortcut* shortcut = new QShortcut(Qt::Key_Return, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Ok), SLOT(click()));

    shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Cancel), SLOT(click()));

    ChatPage page;
    page.resize(640, 400);

    foreach (const QString& name, ThemeLoader::instance()->themes()) {
        ThemeInfo theme = ThemeLoader::instance()->theme(name);

        QPixmap pixmap(640, 400);
        QPainter painter(&pixmap);

        page.setTheme(theme.name());
        page.render(&painter);

        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        pixmap.scaled(320, 200).save(&buffer, "PNG");

        QLabel* label = new QLabel(ui.content);
        label->setTextFormat(Qt::RichText);
        label->setText(tr("<table><tr>"
                          "<td>"
                          "<img src='data:image/png;base64,%5'/>"
                          "</td>"
                          "<td>"
                          "<h1>%1</h1>"
                          "<p><b>Version</b>: %2</p>"
                          "<p><b>Author</b>: %3</p>"
                          "<p>%4</p>"
                          "</td>"
                          "</tr></table>").arg(theme.name(), theme.version(), theme.author(), theme.description(), ba.toBase64()));
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
