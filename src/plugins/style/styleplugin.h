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
#include "listviewplugin.h"
#include "textbrowserplugin.h"
#include "textdocumentplugin.h"
#include "textinputplugin.h"
#include "topiclabelplugin.h"
#include "treewidgetplugin.h"
#include "windowplugin.h"

class QtDockTile;

class StylePlugin : public QObject, public TextBrowserPlugin,
                                    public TextDocumentPlugin,
                                    public TextInputPlugin,
                                    public ListViewPlugin,
                                    public TopicLabelPlugin,
                                    public TreeWidgetPlugin,
                                    public WindowPlugin
{
    Q_OBJECT
    Q_INTERFACES(TextBrowserPlugin TextDocumentPlugin TextInputPlugin ListViewPlugin TopicLabelPlugin TreeWidgetPlugin WindowPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.TextBrowserPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TextDocumentPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TextInputPlugin")
    Q_PLUGIN_METADATA(IID "Communi.ListViewPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TopicLabelPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TreeWidgetPlugin")
    Q_PLUGIN_METADATA(IID "Communi.WindowPlugin")
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
