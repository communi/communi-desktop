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

#include "titleplugin.h"
#include "topiceditor.h"
#include "bufferview.h"
#include "titlebar.h"
#include <QActionEvent>
#include <QAction>
#include <QEvent>
#include <QMenu>

TitlePlugin::TitlePlugin(QObject* parent) : QObject(parent)
{
    d.menu = false;
}

void TitlePlugin::initialize(BufferView* view)
{
    d.bar = view->titleBar();
    d.bar->setAttribute(Qt::WA_Hover);
    d.bar->installEventFilter(this);

    new TopicEditor(d.bar);

    d.closeButton = new QToolButton(d.bar);
    d.closeButton->setObjectName("close");
    d.closeButton->setVisible(false);
    d.closeButton->adjustSize();
    connect(d.closeButton, SIGNAL(clicked()), view, SLOT(closeBuffer()));

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
}

bool TitlePlugin::eventFilter(QObject* object, QEvent* event)
{
    switch (event->type()) {
    case QEvent::Enter:
    case QEvent::Leave:
        updateButtons();
        break;
    case QEvent::ActionAdded:
        d.menuButton->menu()->addAction(static_cast<QActionEvent*>(event)->action());
        break;
    case QEvent::ActionRemoved:
        d.menuButton->menu()->removeAction(static_cast<QActionEvent*>(event)->action());
        break;
    case QEvent::Resize: {
        QRect r = d.closeButton->rect();
        r.moveTopRight(d.bar->rect().topRight());
        d.closeButton->setGeometry(r);
        r.moveTopRight(d.closeButton->geometry().topLeft() - QPoint(1, 0));
        d.menuButton->setGeometry(r);
        break;
    }
    default:
        break;
    }
    return QObject::eventFilter(object, event);
}

void TitlePlugin::aboutToShowMenu()
{
    d.menu = true;
    updateButtons();
}

void TitlePlugin::aboutToHideMenu()
{
    d.menu = false;
    updateButtons();
}

void TitlePlugin::updateButtons()
{
    bool hover = d.bar->underMouse();
    d.closeButton->setVisible(d.menu || hover);
    d.menuButton->setVisible(d.menu || (hover && !d.menuButton->menu()->actions().isEmpty()));
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(TitlePlugin)
#endif
