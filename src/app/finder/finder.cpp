/*
  Copyright (C) 2008-2015 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    d.lastSearch = NoSearch;

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
    connect(d.nextShortcut, SIGNAL(activated()), this, SLOT(findNext()));
    connect(d.prevShortcut, SIGNAL(activated()), this, SLOT(findPrevious()));
}

void Finder::searchTree()
{
    cancelListSearch();
    cancelBrowserSearch();
    d.lastSearch = TreeSearch;
    AbstractFinder* finder = d.page->treeWidget()->findChild<TreeFinder*>();
    if (!finder)
        startSearch(new TreeFinder(d.page->treeWidget()), d.treeSearch);
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
        d.lastSearch = ListSearch;
        AbstractFinder* finder = view->listView()->findChild<ListFinder*>();
        if (!finder)
            startSearch(new ListFinder(view->listView()), d.listSearch);
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
        d.lastSearch = BrowserSearch;
        AbstractFinder* finder = view->textBrowser()->findChild<BrowserFinder*>();
        if (!finder)
            startSearch(new BrowserFinder(view->textBrowser()), d.browserSearch);
        else if (!finder->isAncestorOf(qApp->focusWidget()))
            finder->reFind();
    }
}

void Finder::findAgain()
{
    switch (d.lastSearch) {
    case TreeSearch:
        searchTree();
        break;
    case ListSearch:
        searchList();
        break;
    case BrowserSearch:
        searchBrowser();
        break;
    case NoSearch:
    default:
        break;
    }
}

void Finder::findNext()
{
    if (!d.currentFinder)
        findAgain();
    if (d.currentFinder)
        d.currentFinder->findNext();
}

void Finder::findPrevious()
{
    if (!d.currentFinder)
        findAgain();
    if (d.currentFinder)
        d.currentFinder->findPrevious();
}

void Finder::startSearch(AbstractFinder* finder, const QString& text)
{
    connect(finder, SIGNAL(destroyed(AbstractFinder*)), this, SLOT(finderDestroyed(AbstractFinder*)));
    d.cancelShortcut->setEnabled(true);
    d.finders.insert(finder);
    d.currentFinder = finder;

    finder->setText(text);
    finder->doFind();
}

void Finder::cancelTreeSearch()
{
    AbstractFinder* finder = d.page->treeWidget()->findChild<TreeFinder*>();
    if (finder) {
        d.treeSearch = finder->text();
        finder->animateHide();
    }
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
        if (finder) {
            d.listSearch = finder->text();
            finder->animateHide();
        }
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
        if (finder) {
            d.browserSearch = finder->text();
            finder->animateHide();
        }
        view->textBrowser()->moveCursorToBottom();
        view->textBrowser()->scrollToBottom();
        if (restoreFocus)
            view->textInput()->setFocus();
    }
}

void Finder::finderDestroyed(AbstractFinder* input)
{
    d.finders.remove(input);
    d.cancelShortcut->setEnabled(!d.finders.isEmpty());
}
