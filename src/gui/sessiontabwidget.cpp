/*
* Copyright (C) 2008-2013 Communi authors
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

#include "sessiontabwidget.h"
#include "channelitem.h"
#include "serveritem.h"
#include "messageview.h"
#include "session.h"
#include <irccommand.h>

SessionTabWidget::SessionTabWidget(Session* session, QWidget* parent) : QStackedWidget(parent)
{
    d.model.setSession(session);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));

    connect(&d.model, SIGNAL(itemAdded(SessionItem*)), this, SLOT(addView(SessionItem*)));

    MessageView* view = addView(d.model.server());
    setCurrentWidget(view);
}

Session* SessionTabWidget::session() const
{
    return qobject_cast<Session*>(d.model.session());
}

MessageView* SessionTabWidget::currentView() const
{
    return qobject_cast<MessageView*>(currentWidget());
}

MessageView* SessionTabWidget::viewAt(int index) const
{
    return qobject_cast<MessageView*>(widget(index));
}

QByteArray SessionTabWidget::saveSplitter() const
{
    foreach (MessageView* view, d.views) {
        if (view->viewType() != MessageView::ServerView)
            return view->saveSplitter();
    }
    return QByteArray();
}

void SessionTabWidget::restoreSplitter(const QByteArray& state)
{
    foreach (MessageView* view, d.views) {
        view->blockSignals(true);
        view->restoreSplitter(state);
        view->blockSignals(false);
    }
    emit splitterChanged(state);
}

MessageView* SessionTabWidget::addView(const QString& receiver)
{
    QString lower = receiver.toLower();
    if (!d.views.contains(lower))
        d.model.addItem(receiver);
    return d.views.value(lower);
}

MessageView* SessionTabWidget::addView(SessionItem* item)
{
    MessageView* view = new MessageView(item, this);
    connect(view, SIGNAL(queried(QString)), this, SLOT(addView(QString)));
    connect(view, SIGNAL(queried(QString)), this, SLOT(openView(QString)));
    connect(view, SIGNAL(messaged(QString,QString)), this, SLOT(sendMessage(QString,QString)));
    connect(view, SIGNAL(splitterChanged(QByteArray)), this, SLOT(restoreSplitter(QByteArray)));

    d.views.insert(item->name().toLower(), view);
    addWidget(view);
    emit viewAdded(view);

    if (qobject_cast<ChannelItem*>(item))
        openView(item->name());

    return view;
}

void SessionTabWidget::openView(const QString& receiver)
{
    MessageView* view = d.views.value(receiver.toLower());
    if (view)
        setCurrentWidget(view);
}

void SessionTabWidget::removeView(const QString& receiver)
{
    MessageView* view = d.views.take(receiver.toLower());
    if (view) {
        view->deleteLater();
        emit viewRemoved(view);
        d.model.removeItem(view->item()->name());
    }
}

void SessionTabWidget::closeView(int index)
{
    MessageView* view = viewAt(index);
    if (view) {
        if (view->item()->isActive()) {
            QString reason = tr("%1 %2").arg(QApplication::applicationName())
                             .arg(QApplication::applicationVersion());
            if (indexOf(view) == 0)
                session()->quit(reason);
            else if (view->viewType() == MessageView::ChannelView)
                d.model.session()->sendCommand(IrcCommand::createPart(view->item()->name(), reason));
        }
        removeView(view->item()->name());
    }
}

void SessionTabWidget::renameView(const QString& from, const QString& to)
{
    MessageView* view = d.views.take(from.toLower());
    if (view) {
        view->item()->setName(to);
        d.views.insert(to.toLower(), view);
        emit viewRenamed(view);
    }
}

void SessionTabWidget::sendMessage(const QString& receiver, const QString& message)
{
    MessageView* view = addView(receiver);
    if (view) {
        setCurrentWidget(view);
        view->sendMessage(message);
    }
}

void SessionTabWidget::tabActivated(int index)
{
    MessageView* view = viewAt(index);
    if (view && isVisible()) {
        d.model.setCurrentItem(view->item());
        view->setFocus();
        emit viewActivated(view);
    }
}
