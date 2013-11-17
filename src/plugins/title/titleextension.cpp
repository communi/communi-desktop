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

#include "titleextension.h"
#include "titlebar.h"
#include <QActionEvent>
#include <QAction>
#include <QEvent>
#include <QMenu>

TitleExtension::TitleExtension(TitleBar* bar) : QObject(bar)
{
    d.menu = false;

    d.bar = bar;
    d.bar->setAttribute(Qt::WA_Hover);
    d.bar->installEventFilter(this);

    d.closeButton = new QToolButton(d.bar);
    d.closeButton->setObjectName("close");
    d.closeButton->setVisible(false);
    d.closeButton->adjustSize();
    connect(d.closeButton, SIGNAL(clicked()), bar->parentWidget(), SLOT(closeBuffer()));

    d.menuButton = new QToolButton(d.bar);
    d.menuButton->setObjectName("menu");
    d.menuButton->setPopupMode(QToolButton::InstantPopup);
    d.menuButton->setVisible(false);
    d.menuButton->adjustSize();

    QMenu* menu = new QMenu(d.menuButton);
    connect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowMenu()));
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideMenu()));
    menu->addActions(d.bar->actions());
    d.menuButton->setMenu(menu);

    updateButtons();
}

bool TitleExtension::eventFilter(QObject* object, QEvent* event)
{
    switch (event->type()) {
    case QEvent::Enter:
    case QEvent::Leave:
    case QEvent::Resize:
        updateButtons();
        break;
    case QEvent::ActionAdded:
        d.menuButton->menu()->addAction(static_cast<QActionEvent*>(event)->action());
        break;
    case QEvent::ActionRemoved:
        d.menuButton->menu()->removeAction(static_cast<QActionEvent*>(event)->action());
        break;
    default:
        break;
    }
    return QObject::eventFilter(object, event);
}

void TitleExtension::aboutToShowMenu()
{
    d.menu = true;
    updateButtons();
}

void TitleExtension::aboutToHideMenu()
{
    d.menu = false;
    updateButtons();
}

void TitleExtension::updateButtons()
{
    bool hover = d.bar->underMouse();
    d.closeButton->setVisible(d.menu || hover);
    d.menuButton->setVisible(d.menu || (hover && !d.menuButton->menu()->actions().isEmpty()));

    QRect r = d.closeButton->rect();
    r.moveTopRight(d.bar->rect().topRight());
    d.closeButton->setGeometry(r);
    r.moveTopRight(d.closeButton->geometry().topLeft() - QPoint(1, 0));
    d.menuButton->setGeometry(r);
}
