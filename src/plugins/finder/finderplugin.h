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

#ifndef FINDERPLUGIN_H
#define FINDERPLUGIN_H

#include <QSet>
#include <QObject>
#include <QtPlugin>
#include <QShortcut>
#include "splitviewplugin.h"
#include "treewidgetplugin.h"

class Finder;

class FinderPlugin : public QObject, public SplitViewPlugin, public TreeWidgetPlugin
{
    Q_OBJECT
    Q_INTERFACES(SplitViewPlugin TreeWidgetPlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "Communi.SplitViewPlugin")
    Q_PLUGIN_METADATA(IID "Communi.TreeWidgetPlugin")
#endif

public:
    explicit FinderPlugin(QObject* parent = 0);

    void initialize(SplitView* view);
    void initialize(TreeWidget* tree);

private slots:
    void searchTree();
    void searchBrowser();
    void cancelTreeSearch();
    void cancelBrowserSearch();
    void startSearch(Finder* input);
    void finderDestroyed(Finder* input);

private:
    struct Private {
        SplitView* view;
        TreeWidget* tree;
        QSet<Finder*> finders;
        QShortcut* nextShortcut;
        QShortcut* prevShortcut;
        QShortcut* cancelShortcut;
    } d;
};

#endif // FINDERPLUGIN_H
