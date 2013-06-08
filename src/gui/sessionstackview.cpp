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
#include "session.h"

SessionStackView::SessionStackView(QWidget* parent) : QStackedWidget(parent)
{
}

QList<IrcSession*> SessionStackView::sessions() const
{
    QList<IrcSession*> list;
    for (int i = 0; i < count(); ++i) {
        MessageStackView* widget = widgetAt(i);
        if (widget)
            list += widget->session();
    }
    return list;
}

int SessionStackView::addSession(IrcSession* session)
{
    MessageStackView* widget = new MessageStackView(session, this);
    return addWidget(widget);
}

void SessionStackView::removeSession(IrcSession* session)
{
    MessageStackView* widget = sessionWidget(session);
    if (widget) {
        removeWidget(widget);
        widget->deleteLater();
        if (Session* session = qobject_cast<Session*>(widget->session()))
            session->destructLater();
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

MessageStackView* SessionStackView::sessionWidget(IrcSession* session) const
{
    for (int i = 0; i < count(); ++i) {
        MessageStackView* widget = widgetAt(i);
        if (widget && widget->session() == session)
            return widget;
    }
    return 0;
}
