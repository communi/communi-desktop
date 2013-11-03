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

#ifndef MENUPLUGIN_H
#define MENUPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "treeplugin.h"
#include "listplugin.h"
#include "browserplugin.h"

class TreeItem;

class MenuPlugin : public QObject, public TreePlugin, public ListPlugin, public BrowserPlugin
{
    Q_OBJECT
    Q_INTERFACES(TreePlugin ListPlugin)
    Q_PLUGIN_METADATA(IID "com.github.communi.TreePlugin")
    Q_PLUGIN_METADATA(IID "com.github.communi.ListPlugin")
    Q_PLUGIN_METADATA(IID "com.github.communi.BrowserPlugin")

public:
    MenuPlugin(QObject* parent = 0);

    void initialize(TreeWidget* tree);
    void initialize(ListView* tree);
    void initialize(TextBrowser* browser);
};

#endif // MENUPLUGIN_H
