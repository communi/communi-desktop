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

#include "finder.h"
#include "chatpage.h"
#include "browserfinder.h"
#include "textbrowser.h"
#include "treewidget.h"
#include "treefinder.h"
#include "listfinder.h"
#include "bufferview.h"
#include "textinput.h"
#include "listview.h"
#include <QApplication>
#include <QTimer>

Finder::Finder(ChatPage* page) : QObject(page)
{
    d.page = page;
    d.nextShortcut = 0;
    d.prevShortcut = 0;

    QShortcut* shortcut = new QShortcut(QKeySequence::Find, page);
    connect(shortcut, SIGNAL(activated()), this, SLOT(searchBrowser()));

    shortcut = new QShortcut(QKeySequence("Ctrl+S"), page);
    connect(shortcut, SIGNAL(activated()), this, SLOT(searchTree()));

    shortcut = new QShortcut(QKeySequence("Ctrl+U"), page);
    connect(shortcut, SIGNAL(activated()), this, SLOT(searchList()));

    d.cancelShortcut = new QShortcut(Qt::Key_Escape, page);
    d.cancelShortcut->setEnabled(false);
    connect(d.cancelShortcut, SIGNAL(activated()), this, SLOT(cancelTreeSearch()));
    connect(d.cancelShortcut, SIGNAL(activated()), this, SLOT(cancelListSearch()));
    connect(d.cancelShortcut, SIGNAL(activated()), this, SLOT(cancelBrowserSearch()));

    d.nextShortcut = new QShortcut(QKeySequence::FindNext, page);
    d.prevShortcut = new QShortcut(QKeySequence::FindPrevious, page);
}

void Finder::searchTree()
{
    cancelListSearch();
    cancelBrowserSearch();
    AbstractFinder* finder = d.page->treeWidget()->findChild<TreeFinder*>();
    if (!finder)
        startSearch(new TreeFinder(d.page->treeWidget()));
    else if (!finder->isAncestorOf(qApp->focusWidget()))
        finder->reFind();
}

void Finder::searchList(BufferView* view)
{
    if (!view)
        view = d.page->currentView();
    if (view && view->listView()->isVisible()) {
        cancelTreeSearch();
        cancelBrowserSearch();
        AbstractFinder* finder = view->listView()->findChild<ListFinder*>();
        if (!finder)
            startSearch(new ListFinder(view->listView()));
        else if (!finder->isAncestorOf(qApp->focusWidget()))
            finder->reFind();
    }
}

void Finder::searchBrowser(BufferView* view)
{
    if (!view)
        view = d.page->currentView();
    if (view && view->isVisible()) {
        cancelListSearch();
        cancelTreeSearch();
        AbstractFinder* finder = view->textBrowser()->findChild<BrowserFinder*>();
        if (!finder)
            startSearch(new BrowserFinder(view->textBrowser()));
        else if (!finder->isAncestorOf(qApp->focusWidget()))
            finder->reFind();
    }
}

void Finder::startSearch(AbstractFinder* finder)
{
    connect(finder, SIGNAL(destroyed(AbstractFinder*)), this, SLOT(finderDestroyed(AbstractFinder*)));
    d.cancelShortcut->setEnabled(true);
    d.finders.insert(finder);

    connect(d.nextShortcut, SIGNAL(activated()), finder, SLOT(findNext()));
    connect(d.prevShortcut, SIGNAL(activated()), finder, SLOT(findPrevious()));
    finder->doFind();
}

void Finder::cancelTreeSearch()
{
    AbstractFinder* finder = d.page->treeWidget()->findChild<TreeFinder*>();
    if (finder)
        finder->animateHide();
    BufferView* view = d.page->currentView();
    if (view)
        view->textInput()->setFocus();
}

void Finder::cancelListSearch(BufferView* view)
{
    bool restoreFocus = !view;
    if (!view)
        view = d.page->currentView();
    if (view) {
        AbstractFinder* finder = view->listView()->findChild<ListFinder*>();
        if (finder)
            finder->animateHide();
        if (restoreFocus)
            view->textInput()->setFocus();
    }
}

void Finder::cancelBrowserSearch(BufferView* view)
{
    bool restoreFocus = !view;
    if (!view)
        view = d.page->currentView();
    if (view) {
        AbstractFinder* finder = view->textBrowser()->findChild<BrowserFinder*>();
        if (finder)
            finder->animateHide();
        if (restoreFocus)
            view->textInput()->setFocus();
    }
}

void Finder::finderDestroyed(AbstractFinder* input)
{
    d.finders.remove(input);
    d.cancelShortcut->setEnabled(!d.finders.isEmpty());
}
