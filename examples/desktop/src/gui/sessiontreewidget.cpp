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
#include "session.h"

class SessionTreeItem : public QTreeWidgetItem
{
public:
    SessionTreeItem(QTreeWidget* parent) : QTreeWidgetItem(parent),
      _alerted(false), _inactive(true), _highlighted(false) { }

    QVariant data(int column, int role) const
    {
        if (role == Qt::ForegroundRole)
        {
            if (_alerted)
                return QColor(Qt::red); // TODO: treeWidget()->palette().color(QPalette::Highlight);
            if (_highlighted)
                return treeWidget()->palette().color(QPalette::Highlight);
            if (_inactive)
                return treeWidget()->palette().color(QPalette::Disabled, QPalette::Highlight);
            return treeWidget()->palette().color(QPalette::WindowText);
        }
        return QTreeWidgetItem::data(column, role);
    }

    void setAlerted(bool alerted) { _alerted = alerted; emitDataChanged(); }
    void setInactive(bool inactive) { _inactive = inactive; emitDataChanged(); }
    void setHighlighted(bool highlighted) { _highlighted = highlighted; emitDataChanged(); }

private:
    bool _alerted;
    bool _inactive;
    bool _highlighted;
};

SessionTreeWidget::SessionTreeWidget(QWidget* parent) : QTreeWidget(parent)
{
    setAnimated(true);
    setHeaderHidden(true);
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
