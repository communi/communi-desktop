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

#ifndef THEMEPLUGIN_H
#define THEMEPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "theme.h"
#include "mainwindowplugin.h"
#include "treewidgetplugin.h"
#include "textdocumentplugin.h"
#include "bufferviewplugin.h"

class ThemePlugin : public QObject, public MainWindowPlugin, public TreeWidgetPlugin, public TextDocumentPlugin, public BufferViewPlugin
{
    Q_OBJECT
    Q_INTERFACES(MainWindowPlugin TreeWidgetPlugin TextDocumentPlugin BufferViewPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.MainWindowPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TreeWidgetPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TextDocumentPlugin")
    Q_PLUGIN_METADATA(IID "Communi.BufferViewPlugin")
#endif

public:
    ThemePlugin(QObject* parent = 0);

    void initialize(MainWindow* window);
    void initialize(TreeWidget* tree);
    void initialize(TextDocument* doc);
    void initialize(BufferView* view);

private:
    struct Private {
        Theme theme;
    } d;
};

#endif // THEMEPLUGIN_H
