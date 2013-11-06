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

#ifndef STYLEPLUGIN_H
#define STYLEPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "browserplugin.h"
#include "documentplugin.h"
#include "inputplugin.h"
#include "listplugin.h"
#include "topicplugin.h"
#include "treeplugin.h"
#include "windowplugin.h"

class QtDockTile;

class StylePlugin : public QObject, public BrowserPlugin,
                                    public DocumentPlugin,
                                    public InputPlugin,
                                    public ListPlugin,
                                    public TopicPlugin,
                                    public TreePlugin,
                                    public WindowPlugin
{
    Q_OBJECT
    Q_INTERFACES(BrowserPlugin DocumentPlugin InputPlugin ListPlugin TopicPlugin TreePlugin WindowPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.github.communi.BrowserPlugin")
    Q_PLUGIN_METADATA(IID "com.github.communi.DocumentPlugin")
    Q_PLUGIN_METADATA(IID "com.github.communi.InputPlugin")
    Q_PLUGIN_METADATA(IID "com.github.communi.ListPlugin")
    Q_PLUGIN_METADATA(IID "com.github.communi.TopicPlugin")
    Q_PLUGIN_METADATA(IID "com.github.communi.TreePlugin")
    Q_PLUGIN_METADATA(IID "com.github.communi.WindowPlugin")
#endif

public:
    StylePlugin(QObject* parent = 0);

    void initialize(TextBrowser* browser);
    void initialize(TextDocument* doc);
    void initialize(TextInput* input);
    void initialize(ListView* list);
    void initialize(TopicLabel* label);
    void initialize(TreeWidget* tree);
    void initialize(QWidget* window);
};

#endif // STYLEPLUGIN_H
