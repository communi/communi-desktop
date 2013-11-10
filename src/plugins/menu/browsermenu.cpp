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

#include "browsermenu.h"
#include "textbrowser.h"
#include "bufferview.h"
#include "splitview.h"
#include <QContextMenuEvent>

BrowserMenu::BrowserMenu(TextBrowser* browser, SplitView* view) : QMenu(browser)
{
    d.view = view;
    d.browser = browser;
    browser->viewport()->installEventFilter(this);

    connect(d.view, SIGNAL(viewAdded(BufferView*)), this, SLOT(updateActions()));
    connect(d.view, SIGNAL(viewRemoved(BufferView*)), this, SLOT(updateActions()));

    d.splitVAction = new QAction(tr("Split"), browser);
    connect(d.splitVAction, SIGNAL(triggered()), this, SLOT(splitVertical()));

    d.splitHAction = new QAction(tr("Split side by side"), browser);
    connect(d.splitHAction, SIGNAL(triggered()), this, SLOT(splitHorizontal()));

    d.closeAction = new QAction(tr("Close"), browser);
    connect(d.closeAction, SIGNAL(triggered()), this, SLOT(unsplit()));

    updateActions();
}

bool BrowserMenu::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::ContextMenu) {
        QContextMenuEvent* cme = static_cast<QContextMenuEvent*>(event);
        const QString anchor = d.browser->anchorAt(cme->pos());
        if (anchor.startsWith("nick:")) {
            QTextCursor cursor = d.browser->cursorForPosition(cme->pos());
            cursor.select(QTextCursor::WordUnderCursor);
            d.browser->setTextCursor(cursor);
        }
        QMenu* menu = d.browser->createStandardContextMenu(cme->pos());
        menu->addSeparator();
        menu->addAction(d.splitVAction);
        menu->addAction(d.splitHAction);
        menu->addAction(d.closeAction);
        menu->exec(cme->globalPos());
        delete menu;
        return true;
    }
    return false;
}

void BrowserMenu::updateActions()
{
    d.closeAction->setEnabled(d.view->views().count() > 1);
}

void BrowserMenu::splitVertical()
{
    d.view->split(Qt::Vertical);
}

void BrowserMenu::splitHorizontal()
{
    d.view->split(Qt::Horizontal);
}

void BrowserMenu::unsplit()
{
    QWidget* parent = d.browser->parentWidget();
    while (parent) {
        if (qobject_cast<BufferView*>(parent)) {
            parent->deleteLater();
            break;
        }
        parent = parent->parentWidget();
    }
}
