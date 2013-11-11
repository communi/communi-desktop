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

#ifndef STATEPLUGIN_H
#define STATEPLUGIN_H

#include <QtPlugin>
#include "windowplugin.h"
#include "splitviewplugin.h"
#include "treewidgetplugin.h"

class StatePlugin : public QObject, public WindowPlugin, public SplitViewPlugin, public TreeWidgetPlugin
{
    Q_OBJECT
    Q_INTERFACES(WindowPlugin SplitViewPlugin TreeWidgetPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.WindowPlugin")
    Q_PLUGIN_METADATA(IID "Communi.SplitViewPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TreeWidgetPlugin")
#endif

public:
    StatePlugin(QObject* parent = 0);

    void initialize(QWidget* window);
    void uninitialize(QWidget* window);

    void initialize(SplitView* view);
    void uninitialize(SplitView* view);

    void initialize(TreeWidget* tree);
    void uninitialize(TreeWidget* tree);
};

#endif // STATEPLUGIN_H
