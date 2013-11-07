/*
* Copyright (C) 2008-2013 The Communi Project
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

#ifndef DOCKPLUGIN_H
#define DOCKPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "windowplugin.h"

class QtDockTile;

class DockPlugin : public QObject, public WindowPlugin
{
    Q_OBJECT
    Q_INTERFACES(WindowPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.WindowPlugin")
#endif

public:
    DockPlugin(QObject* parent = 0);

    void initialize(QWidget* window);

private:
    struct Private {
        QWidget* window;
        QtDockTile* dock;
    } d;
};

#endif // DOCKPLUGIN_H
