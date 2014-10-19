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
#include "gnomemenu.h"
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

#ifdef COMMUNI_HAVE_GIO
    GnomeMenu *section1 = new GnomeMenu(window);
    section1->addSimpleItem("showConnect", "Connect...", window, "doConnect");
    section1->addSimpleItem("showSettings", "Settings", window, "showSettings");
    section1->addSimpleItem("showHelp", "Help", window, "showHelp");

    GnomeMenu *section2 = new GnomeMenu(window);
    section2->addToggleItem("toggleMute", "Mute", d.mute->isChecked(), d.mute, "toggle");

    GnomeMenu *section3 = new GnomeMenu(window);
    section3->addSimpleItem("quit", "Quit", window, "close");

    GnomeMenu *builder = new GnomeMenu(window);
    builder->addSection(section1);
    builder->addSection(section2);
    builder->addSection(section3);
    builder->setMenuToWindow(window->winId(), "/org/communi/gnomeintegration");
#endif // COMMUNI_HAVE_GIO
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
