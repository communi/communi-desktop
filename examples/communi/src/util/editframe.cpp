/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#include "editframe.h"
#include "completer.h"
#include "application.h"
#include <QShortcut>
#include <QMenu>

EditFrame::EditFrame(QWidget* parent) : QFrame(parent)
{
    ui.setupUi(this);

    ui.completer = new Completer(this);
    ui.completer->setWidget(this);
    ui.completer->setLineEdit(ui.lineEdit);
    QShortcut* shortcut = new QShortcut(Qt::Key_Tab, this);
    connect(shortcut, SIGNAL(activated()), ui.completer, SLOT(onTabPressed()));

    ui.tabButton->setFocusProxy(ui.lineEdit);
    ui.helpButton->setFocusProxy(ui.lineEdit);
    ui.sendButton->setFocusProxy(ui.lineEdit);

    static bool noHelpButton = qgetenv("COMMUNI_NO_HELP_BUTTON").toInt()
        || Application::arguments().contains("-no-help-button")
        || Application::arguments().contains("-nhb");

    if (noHelpButton)
        ui.helpButton->hide();

    QMenu* helpMenu = new QMenu(ui.helpButton);
    foreach (const QString& category, Application::commandCategories())
    {
        QMenu* categoryMenu = helpMenu->addMenu(category);
        Commands commands = Application::commands(category);
        foreach (const Command& command, commands)
            categoryMenu->addAction(command.name);
    }
    ui.helpButton->setMenu(helpMenu);

    connect(helpMenu, SIGNAL(triggered(QAction*)), this, SLOT(commandAction(QAction*)));
    connect(ui.helpButton, SIGNAL(clicked()), this, SIGNAL(help()));
    connect(ui.tabButton, SIGNAL(clicked()), ui.completer, SLOT(onTabPressed()));
    connect(ui.sendButton, SIGNAL(clicked()), this, SLOT(onSend()));
    connect(ui.lineEdit, SIGNAL(returnPressed()), this, SLOT(onSend()));
    connect(ui.lineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));

    updateUi();
}

Completer* EditFrame::completer() const
{
    return ui.completer;
}

HistoryLineEdit* EditFrame::lineEdit() const
{
    return ui.lineEdit;
}

void EditFrame::hideEvent(QHideEvent* event)
{
    QFrame::hideEvent(event);
}

void EditFrame::onSend()
{
    QString input = ui.lineEdit->text();
    if (!input.isEmpty())
    {
        ui.lineEdit->clear();
        emit send(input);
    }
}

void EditFrame::updateUi()
{
    QString input = ui.lineEdit->text();
    ui.tabButton->setEnabled(!input.isEmpty());
    ui.sendButton->setEnabled(!input.isEmpty());
}

void EditFrame::commandAction(QAction* action)
{
    QString input = QString("/%1").arg(action->text());
    if (!ui.lineEdit->text().isEmpty())
        input = QString("%1 %2").arg(input, ui.lineEdit->text());
    ui.lineEdit->setText(input);
    emit help(action->text());
}
