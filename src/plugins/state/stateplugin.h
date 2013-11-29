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
#include <QVariant>
#include <QSplitter>
#include "splitviewplugin.h"
#include "treewidgetplugin.h"

class IrcBuffer;
class BufferView;

class StatePlugin : public QObject, public SplitViewPlugin, public TreeWidgetPlugin
{
    Q_OBJECT
    Q_INTERFACES(SplitViewPlugin TreeWidgetPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.SplitViewPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TreeWidgetPlugin")
#endif

public:
    StatePlugin(QObject* parent = 0);

    void initView(SplitView* view);
    void cleanupView(SplitView* view);

    void initTree(TreeWidget* tree);

private slots:
    void restoreBuffer(IrcBuffer* buffer);

private:
    QVariantMap saveSplittedViews(QSplitter* splitter) const;
    void restoreSplittedViews(QSplitter* splitter, const QVariantMap& state, SplitView* view = 0);

    struct Private {
        TreeWidget* tree;
        SplitView* view;
    } d;
};

#endif // STATEPLUGIN_H
