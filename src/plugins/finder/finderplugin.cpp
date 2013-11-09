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

#include "finderplugin.h"
#include "browserfinder.h"
#include "textbrowser.h"
#include "treewidget.h"
#include "treefinder.h"
#include "bufferview.h"
#include "textinput.h"
#include "splitview.h"
#include <QApplication>
#include <QTimer>

inline void initResource() { Q_INIT_RESOURCE(finder); }

FinderPlugin::FinderPlugin(QObject* parent) : QObject(parent)
{
    d.view = 0;
    d.nextShortcut = 0;
    d.prevShortcut = 0;

    initResource();
}

void FinderPlugin::initialize(SplitView* view)
{
    d.view = view;

    QShortcut* shortcut = new QShortcut(QKeySequence::Find, view);
    connect(shortcut, SIGNAL(activated()), this, SLOT(searchBrowser()));

    shortcut = new QShortcut(QKeySequence("Ctrl+S"), view);
    connect(shortcut, SIGNAL(activated()), this, SLOT(searchTree()));

    d.cancelShortcut = new QShortcut(QKeySequence("Esc"), view);
    d.cancelShortcut->setEnabled(false);
    connect(d.cancelShortcut, SIGNAL(activated()), this, SLOT(cancelTreeSearch()));
    connect(d.cancelShortcut, SIGNAL(activated()), this, SLOT(cancelBrowserSearch()));

    d.nextShortcut = new QShortcut(QKeySequence::FindNext, view);
    d.prevShortcut = new QShortcut(QKeySequence::FindPrevious, view);
}

void FinderPlugin::initialize(TreeWidget* tree)
{
    d.tree = tree;
}

void FinderPlugin::searchTree()
{
    cancelBrowserSearch();
    Finder* finder = d.tree->findChild<TreeFinder*>();
    if (!finder)
        startSearch(new TreeFinder(d.tree));
    else if (!finder->isAncestorOf(qApp->focusWidget()))
        finder->reFind();
}

void FinderPlugin::searchBrowser()
{
    cancelTreeSearch();
    BufferView* view = d.view->currentView();
    if (view) {
        Finder* finder = view->textBrowser()->findChild<BrowserFinder*>();
        if (!finder)
            startSearch(new BrowserFinder(view->textBrowser()));
        else if (!finder->isAncestorOf(qApp->focusWidget()))
            finder->reFind();
    }
}

void FinderPlugin::startSearch(Finder* finder)
{
    connect(finder, SIGNAL(destroyed(Finder*)), this, SLOT(finderDestroyed(Finder*)));
    d.cancelShortcut->setEnabled(true);
    d.finders.insert(finder);

    connect(d.nextShortcut, SIGNAL(activated()), finder, SLOT(findNext()));
    connect(d.prevShortcut, SIGNAL(activated()), finder, SLOT(findPrevious()));
    finder->doFind();
}

void FinderPlugin::cancelTreeSearch()
{
    Finder* finder = d.tree->findChild<TreeFinder*>();
    if (finder)
        finder->animateHide();
    BufferView* view = d.view->currentView();
    if (view)
        view->textInput()->setFocus();
}

void FinderPlugin::cancelBrowserSearch()
{
    BufferView* view = d.view->currentView();
    if (view) {
        Finder* finder = view->textBrowser()->findChild<BrowserFinder*>();
        if (finder)
            finder->animateHide();
    }
}

void FinderPlugin::finderDestroyed(Finder* input)
{
    d.finders.remove(input);
    d.cancelShortcut->setEnabled(!d.finders.isEmpty());
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(FinderPlugin)
#endif
