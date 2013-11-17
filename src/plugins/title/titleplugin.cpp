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
}

void TitlePlugin::initialize(BufferView* view)
{
    TitleBar* bar = view->titleBar();
    bar->setAttribute(Qt::WA_Hover);
    bar->installEventFilter(this);

    new TopicEditor(bar);

    d.closeButton = new QToolButton(bar);
    d.closeButton->setObjectName("close");
    d.closeButton->setVisible(false);
    d.closeButton->adjustSize();
    connect(d.closeButton, SIGNAL(clicked()), view, SLOT(closeBuffer()));

    d.menuButton = new QToolButton(bar);
    d.menuButton->setObjectName("menu");
    d.menuButton->setPopupMode(QToolButton::InstantPopup);
    d.menuButton->setMenu(new QMenu(d.menuButton));
    d.menuButton->menu()->addActions(bar->actions());
    d.menuButton->setVisible(false);
    d.menuButton->adjustSize();
}

bool TitlePlugin::eventFilter(QObject* object, QEvent* event)
{
    TitleBar* bar = qobject_cast<TitleBar*>(object);
    if (bar) {
        switch (event->type()) {
        case QEvent::Enter:
            d.closeButton->setVisible(true);
            d.menuButton->setVisible(!d.menuButton->menu()->actions().isEmpty());
            break;
        case QEvent::Leave:
            d.closeButton->setVisible(false);
            d.menuButton->setVisible(false);
            break;
        case QEvent::ActionAdded:
            d.menuButton->menu()->addAction(static_cast<QActionEvent*>(event)->action());
            break;
        case QEvent::ActionRemoved:
            d.menuButton->menu()->removeAction(static_cast<QActionEvent*>(event)->action());
            break;
        case QEvent::Resize: {
            QRect r = d.closeButton->rect();
            r.moveTopRight(bar->rect().topRight());
            d.closeButton->setGeometry(r);
            r.moveTopRight(d.closeButton->geometry().topLeft() - QPoint(1, 0));
            d.menuButton->setGeometry(r);
            break;
        }
        default:
            break;
        }
    }
    return QObject::eventFilter(object, event);
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(TitlePlugin)
#endif
