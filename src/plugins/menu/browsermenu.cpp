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

BrowserMenu::BrowserMenu(TextBrowser* browser) : QMenu(browser)
{
    d.browser = browser;
    browser->installEventFilter(this);
}

bool BrowserMenu::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::ContextMenu) {
        QContextMenuEvent* cme = static_cast<QContextMenuEvent*>(event);
        QMenu* menu = d.browser->createStandardContextMenu(cme->pos());
        if (!actions().isEmpty()) {
            menu->addSeparator();
            menu->addActions(actions());
        }
        menu->exec(cme->globalPos());
        delete menu;
        return true;
    }
    return false;
}

/*
TODO:
QAction* splitAction = menu->addAction(tr("Split"));
QAction* splitSideAction = menu->addAction(tr("Split side by side"));
QAction* closeAction = menu->addAction(tr("Close"));
closeAction->setEnabled(window()->findChildren<BufferView*>().count() > 1);
QAction* action = menu->exec(event->globalPos());
if (action == splitAction)
    emit split(Qt::Vertical);
else if (action == splitSideAction)
    emit split(Qt::Horizontal);
else if (action == closeAction)
    parent()->parent()->deleteLater();
delete menu;
*/
