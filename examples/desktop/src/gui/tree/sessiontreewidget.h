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

#include "treewidget.h"
#include <QHash>

class Session;
class SessionTreeItem;

class SessionTreeWidget : public TreeWidget
{
    Q_OBJECT

public:
    SessionTreeWidget(QWidget* parent = 0);

    QList<Session*> sessions() const;
    void addSession(Session* session);
    void removeSession(Session* session);

    void setInactive(Session* session, bool inactive);
    void setAlerted(Session* session, bool alerted);
    void setHighlighted(Session* session, bool highlighted);

private slots:
    void onSessionNetworkChanged(const QString& network);

private:
    struct SessionTreeWidgetData
    {
        QHash<Session*, SessionTreeItem*> sessions;
    } d;
};

#endif // SESSIONTREEWIDGET_H
