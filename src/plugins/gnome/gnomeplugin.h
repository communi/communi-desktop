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

#ifndef GNOMEPLUGIN_H
#define GNOMEPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "themeplugin.h"
#include "dockplugin.h"

class GnomePlugin : public QObject, public ThemePlugin, public DockPlugin
{
    Q_OBJECT
    Q_INTERFACES(ThemePlugin DockPlugin)
    Q_PLUGIN_METADATA(IID "Communi.ThemePlugin")
    Q_PLUGIN_METADATA(IID "Communi.DockPlugin")

public:
    GnomePlugin(QObject* parent = 0);

    void themeChanged(const ThemeInfo& theme);
    void setupTrayIcon(QSystemTrayIcon* tray);
};

#endif // GNOMEPLUGIN_H
