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

QList<Session*> SessionStackView::sessions() const
{
    QList<Session*> list;
    for (int i = 0; i < count(); ++i) {
        MessageStackView* widget = widgetAt(i);
        if (widget)
            list += widget->session();
    }
    return list;
}

int SessionStackView::addSession(Session* session)
{
    MessageStackView* widget = new MessageStackView(session, this);
    connect(widget, SIGNAL(splitterChanged(QByteArray)), this, SLOT(restoreSplitter(QByteArray)));
    return addWidget(widget);
}

void SessionStackView::removeSession(Session* session)
{
    MessageStackView* widget = sessionWidget(session);
    if (widget) {
        removeWidget(widget);
        widget->deleteLater();
        widget->session()->destructLater();
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

MessageStackView* SessionStackView::sessionWidget(Session* session) const
{
    for (int i = 0; i < count(); ++i) {
        MessageStackView* widget = widgetAt(i);
        if (widget && widget->session() == session)
            return widget;
    }
    return 0;
}

QByteArray SessionStackView::saveSplitter() const
{
    QByteArray state;
    for (int i = count(); state.isNull() && i >= 0; --i) {
        MessageStackView* widget = widgetAt(i);
        if (widget)
            state = widget->saveSplitter();
    }
    return state;
}

void SessionStackView::restoreSplitter(const QByteArray& state)
{
    for (int i = 0; i < count(); ++i) {
        MessageStackView* widget = widgetAt(i);
        if (widget) {
            widget->blockSignals(true);
            widget->restoreSplitter(state);
            widget->blockSignals(false);
        }
    }
    emit splitterChanged(state);
}
