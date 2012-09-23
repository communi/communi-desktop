/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
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

#include "sessiontreewidget.h"
#include "sessiontreeitem.h"
#include "session.h"
#include <QContextMenuEvent>

SessionTreeWidget::SessionTreeWidget(QWidget* parent) : QTreeWidget(parent)
{
    setAnimated(true);
    setHeaderHidden(true);

    d.colors[Active] = palette().color(QPalette::WindowText);
    d.colors[Inactive] = palette().color(QPalette::Disabled, QPalette::Highlight);
    d.colors[Alert] = QColor(Qt::red); //palette().color(QPalette::Highlight);
    d.colors[Highlight] = QColor(Qt::green); //palette().color(QPalette::Highlight);
}

QColor SessionTreeWidget::statusColor(SessionTreeWidget::ItemStatus status) const
{
    return d.colors.value(status);
}

void SessionTreeWidget::setStatusColor(SessionTreeWidget::ItemStatus status, const QColor& color)
{
    d.colors[status] = color;
}

QList<Session*> SessionTreeWidget::sessions() const
{
    return d.sessions.keys();
}

void SessionTreeWidget::addSession(Session* session)
{
    SessionTreeItem* item = new SessionTreeItem(this);
    d.sessions.insert(session, item);

    QString name = session->name();
    if (name.isEmpty())
        connect(session, SIGNAL(networkChanged(QString)), this, SLOT(onSessionNetworkChanged(QString)));
    item->setText(0, name.isEmpty() ? session->host() : name);

    setInactive(session, !session->isActive());
}

void SessionTreeWidget::removeSession(Session *session)
{
    delete d.sessions.take(session);
}

void SessionTreeWidget::addView(Session* session, const QString& view)
{
    SessionTreeItem* parent = d.sessions.value(session);
    if (parent)
    {
        SessionTreeItem* item = new SessionTreeItem(parent);
        item->setText(0, view);
    }
}

void SessionTreeWidget::removeView(Session* session, const QString& view)
{
    SessionTreeItem* parent = d.sessions.value(session);
    if (parent)
        delete parent->findChild(view);
}

void SessionTreeWidget::renameView(Session* session, const QString& from, const QString& to)
{
    SessionTreeItem* parent = d.sessions.value(session);
    if (parent)
    {
        SessionTreeItem* item = parent->findChild(from);
        if (item)
            item->setText(0, to);
    }
}

void SessionTreeWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QTreeWidgetItem* item = itemAt(event->pos());
    if (item)
        emit menuRequested(item, event->globalPos());
}

void SessionTreeWidget::onSessionNetworkChanged(const QString& network)
{
    Session* session = qobject_cast<Session*>(sender());
    SessionTreeItem* item = d.sessions.value(session);
    if (item)
        item->setText(0, network);
}

void SessionTreeWidget::setInactive(Session* session, bool inactive)
{
    SessionTreeItem* item = d.sessions.value(session);
    if (item)
        item->setInactive(inactive);
}

void SessionTreeWidget::setAlerted(Session* session, bool alerted)
{
    SessionTreeItem* item = d.sessions.value(session);
    if (item)
        item->setAlerted(alerted);
}

void SessionTreeWidget::setHighlighted(Session* session, bool highlighted)
{
    SessionTreeItem* item = d.sessions.value(session);
    if (item)
        item->setHighlighted(highlighted);
}
