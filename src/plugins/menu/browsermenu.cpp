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
#include <QContextMenuEvent>
#include <QMenu>

BrowserMenu::BrowserMenu(TextBrowser* browser) : QObject(browser)
{
    d.browser = browser;
    browser->viewport()->installEventFilter(this);
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
        menu->exec(cme->globalPos());
        menu->deleteLater();
        return true;
    }
    return false;
}
