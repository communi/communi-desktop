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

#ifndef SESSIONTREEWIDGET_H
#define SESSIONTREEWIDGET_H

#include <QTreeWidget>
#include <QColor>
#include <QHash>

class Session;
class SessionTreeItem;

class SessionTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    SessionTreeWidget(QWidget* parent = 0);

    enum ItemStatus { Active, Inactive, Alert, Highlight };

    QColor statusColor(ItemStatus status) const;
    void setStatusColor(ItemStatus status, const QColor& color);

    QList<Session*> sessions() const;

    void setInactive(Session* session, bool inactive);
    void setAlerted(Session* session, bool alerted);
    void setHighlighted(Session* session, bool highlighted);

public slots:
    void addSession(Session* session);
    void removeSession(Session* session);

signals:
    void menuRequested(QTreeWidgetItem* item, const QPoint& pos);

protected:
    void contextMenuEvent(QContextMenuEvent* event);

private slots:
    void onSessionNetworkChanged(const QString& network);

private:
    struct SessionTreeWidgetData
    {
        QHash<ItemStatus, QColor> colors;
        QHash<Session*, SessionTreeItem*> sessions;
    } d;
};

#endif // SESSIONTREEWIDGET_H
