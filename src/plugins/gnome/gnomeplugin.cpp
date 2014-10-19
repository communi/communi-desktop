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

#include "gnomeplugin.h"
#include "x11helper.h"
#include "themeinfo.h"
#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QAction>

GnomePlugin::GnomePlugin(QObject* parent) : QObject(parent)
{
    d.mute = 0;
    d.window = 0;
}

void GnomePlugin::windowCreated(QMainWindow* window)
{
    d.window = window;
}

void GnomePlugin::themeChanged(const ThemeInfo& theme)
{
    QByteArray gtkTheme = theme.gtkTheme().toUtf8();
    X11Helper::setWindowProperty(d.window->winId(), "_GTK_THEME_VARIANT", gtkTheme);
}

void GnomePlugin::setupTrayIcon(QSystemTrayIcon* tray)
{
    tray->setVisible(false);
}

void GnomePlugin::setupMuteAction(QAction* action)
{
    d.mute = action;
}
