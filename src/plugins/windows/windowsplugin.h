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

#ifndef WINDOWSPLUGIN_H
#define WINDOWSPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "windowplugin.h"

class WindowsPlugin : public QObject, public WindowPlugin
{
    Q_OBJECT
    Q_INTERFACES(WindowPlugin)
    Q_PLUGIN_METADATA(IID "Communi.WindowPlugin")

public:
    WindowsPlugin(QObject* parent = 0);

    void windowCreated(QMainWindow* window);
};

#endif // WINDOWSPLUGIN_H
