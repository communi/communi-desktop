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
    QList<IrcConnection*> list;
    for (int i = 0; i < count(); ++i) {
        MessageStackView* widget = widgetAt(i);
        if (widget)
            list += widget->connection();
    }
    return list;
}

int SessionStackView::addConnection(IrcConnection* connection)
{
    MessageStackView* widget = new MessageStackView(connection, this);
    return addWidget(widget);
}

void SessionStackView::removeConnection(IrcConnection* connection)
{
    MessageStackView* widget = connectionWidget(connection);
    if (widget) {
        removeWidget(widget);
        widget->deleteLater();
        if (Connection* connection = qobject_cast<Connection*>(widget->connection()))
            connection->destructLater();
    }
}

MessageStackView* SessionStackView::currentWidget() const
{
    return qobject_cast<MessageStackView*>(QStackedWidget::currentWidget());
}

MessageStackView* SessionStackView::widgetAt(int index) const
{
    return qobject_cast<MessageStackView*>(QStackedWidget::widget(index));
}

MessageStackView* SessionStackView::connectionWidget(IrcConnection* connection) const
{
    for (int i = 0; i < count(); ++i) {
        MessageStackView* widget = widgetAt(i);
        if (widget && widget->connection() == connection)
            return widget;
    }
    return 0;
}
