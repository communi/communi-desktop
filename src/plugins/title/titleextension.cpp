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
#include <QPropertyAnimation>
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

    d.offset = -d.closeButton->sizeHint().height();
    updateButtons();
}

int TitleExtension::offset() const
{
    return d.offset;
}

void TitleExtension::setOffset(int offset)
{
    d.offset = offset;
    relocate();
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
    if (d.menu || d.bar->underMouse()) {
        d.closeButton->show();
        d.menuButton->setVisible(!d.menuButton->menu()->actions().isEmpty());
        animateShow();
    } else {
        animateHide();
    }
    relocate();
}

void TitleExtension::animateShow()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "offset");
    animation->setDuration(50);
    animation->setEndValue(0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void TitleExtension::animateHide()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "offset");
    animation->setDuration(50);
    animation->setEndValue(-d.closeButton->sizeHint().height());
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation, SIGNAL(destroyed()), d.closeButton, SLOT(hide()));
    connect(animation, SIGNAL(destroyed()), d.menuButton, SLOT(hide()));
}

void TitleExtension::relocate()
{
    QRect r = d.closeButton->rect();
    r.moveTopRight(d.bar->rect().topRight());
    r.translate(0, d.offset);
    d.closeButton->setGeometry(r);
    r.moveRight(r.left() - 1);
    d.menuButton->setGeometry(r);
}
