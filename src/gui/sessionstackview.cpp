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

#include "sessionstackview.h"
#include "messagestackview.h"
#include "messageview.h"
#include "connection.h"

SessionStackView::SessionStackView(QWidget* parent) : QStackedWidget(parent)
{
}

QList<IrcConnection*> SessionStackView::connections() const
{
    return d.connections;
}

int SessionStackView::addConnection(IrcConnection* connection)
{
    MessageStackView* widget = new MessageStackView(connection, this);
    d.connections += connection;
    d.connectionWidgets.insert(connection, widget);
    return addWidget(widget);
}

void SessionStackView::removeConnection(IrcConnection* connection)
{
    MessageStackView* widget = d.connectionWidgets.take(connection);
    if (widget) {
        removeWidget(widget);
        widget->deleteLater();
        d.connections.removeOne(connection);
        if (Connection* connection = qobject_cast<Connection*>(widget->connection())) {
            connection->quit();
            QTimer::singleShot(1000, connection, SLOT(deleteLater()));
        }
    }
}

MessageStackView* SessionStackView::currentWidget() const
{
    return qobject_cast<MessageStackView*>(QStackedWidget::currentWidget());
}

MessageStackView* SessionStackView::connectionWidget(IrcConnection* connection) const
{
    return d.connectionWidgets.value(connection);
}
