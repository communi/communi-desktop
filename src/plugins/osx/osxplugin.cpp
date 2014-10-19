/*
* Copyright (C) 2008-2014 The Communi Project
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

#include "osxplugin.h"
#include <QSystemTrayIcon>
#include <IrcTextFormat>
#include <QMainWindow>
#include <IrcMessage>
#include <QMenuBar>
#include <QMenu>

OsxPlugin::OsxPlugin(QObject* parent) : QObject(parent)
{
    d.tray = 0;
}

void OsxPlugin::windowCreated(QMainWindow* window)
{
    QMenu* menu = new QMenu(window);
    window->menuBar()->addMenu(menu);

    QAction* action = new QAction(tr("Preferences"), window);
    action->setMenuRole(QAction::PreferencesRole);
    connect(action, SIGNAL(triggered()), window, SLOT(showSettings()));
    menu->addAction(action);
}

void OsxPlugin::setupTrayIcon(QSystemTrayIcon* tray)
{
    d.tray = tray;
}

void OsxPlugin::dockAlert(IrcMessage* message)
{
    QString content = message->property("content").toString();
    if (!content.isEmpty())
        d.tray->showMessage(tr("Communi"), message->nick() + ": " + IrcTextFormat().toPlainText(content));
}
