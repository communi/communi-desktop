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
#include "bufferviewplugin.h"
#include "textdocumentplugin.h"
#include "treewidgetplugin.h"
#include "mainwindowplugin.h"

class QtDockTile;

class StylePlugin : public QObject, public BufferViewPlugin,
                                    public TextDocumentPlugin,
                                    public TreeWidgetPlugin,
                                    public MainWindowPlugin
{
    Q_OBJECT
    Q_INTERFACES(BufferViewPlugin TextDocumentPlugin TreeWidgetPlugin MainWindowPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.BufferViewPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TextDocumentPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TreeWidgetPlugin")
    Q_PLUGIN_METADATA(IID "Communi.MainWindowPlugin")
#endif

public:
    StylePlugin(QObject* parent = 0);

    void initialize(BufferView* view);
    void initialize(TextDocument* doc);
    void initialize(TreeWidget* tree);
    void initialize(MainWindow* window);
};

#endif // STYLEPLUGIN_H
