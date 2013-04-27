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

#include "multisessiontabwidget.h"
#include "messagestackview.h"
#include "messageview.h"
#include "session.h"

MultiMessageStackView::MultiMessageStackView(QWidget* parent) : QStackedWidget(parent)
{
}

QList<Session*> MultiMessageStackView::sessions() const
{
    QList<Session*> list;
    for (int i = 0; i < count(); ++i) {
        MessageStackView* tabWidget = qobject_cast<MessageStackView*>(widget(i));
        if (tabWidget)
            list += tabWidget->session();
    }
    return list;
}

int MultiMessageStackView::addSession(Session* session)
{
    MessageStackView* tab = new MessageStackView(session, this);
    connect(tab, SIGNAL(splitterChanged(QByteArray)), this, SLOT(restoreSplitter(QByteArray)));
    return addWidget(tab);
}

void MultiMessageStackView::removeSession(Session* session)
{
    MessageStackView* tabWidget = sessionWidget(session);
    if (tabWidget) {
        removeWidget(tabWidget);
        tabWidget->deleteLater();
        tabWidget->session()->destructLater();
    }
}

MessageStackView* MultiMessageStackView::currentWidget() const
{
    return qobject_cast<MessageStackView*>(QStackedWidget::currentWidget());
}

MessageStackView* MultiMessageStackView::widgetAt(int index) const
{
    return qobject_cast<MessageStackView*>(QStackedWidget::widget(index));
}

MessageStackView* MultiMessageStackView::sessionWidget(Session* session) const
{
    for (int i = 0; i < count(); ++i) {
        MessageStackView* tabWidget = widgetAt(i);
        if (tabWidget && tabWidget->session() == session)
            return tabWidget;
    }
    return 0;
}

QByteArray MultiMessageStackView::saveSplitter() const
{
    QByteArray state;
    for (int i = count(); state.isNull() && i >= 0; --i) {
        MessageStackView* tabWidget = widgetAt(i);
        if (tabWidget)
            state = tabWidget->saveSplitter();
    }
    return state;
}

void MultiMessageStackView::restoreSplitter(const QByteArray& state)
{
    for (int i = 0; i < count(); ++i) {
        MessageStackView* tabWidget = widgetAt(i);
        if (tabWidget) {
            tabWidget->blockSignals(true);
            tabWidget->restoreSplitter(state);
            tabWidget->blockSignals(false);
        }
    }
    emit splitterChanged(state);
}
