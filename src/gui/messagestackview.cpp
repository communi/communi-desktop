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

#include "messagestackview.h"
#include "completer.h"
#include "session.h"
#include <irccommand.h>

MessageStackView::MessageStackView(Session* session, QWidget* parent) : QStackedWidget(parent)
{
    d.handler.setSession(session);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(activateView(int)));

    connect(&d.handler, SIGNAL(receiverToBeAdded(QString)), this, SLOT(addView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRemoved(QString)), this, SLOT(removeView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRenamed(QString, QString)), this, SLOT(renameView(QString, QString)));

    MessageView* view = addView(d.handler.session()->host());
    d.handler.setDefaultReceiver(view);
    d.handler.setCurrentReceiver(view);
    setCurrentWidget(view);
}

Session* MessageStackView::session() const
{
    return qobject_cast<Session*>(d.handler.session());
}

MessageView* MessageStackView::currentView() const
{
    return qobject_cast<MessageView*>(currentWidget());
}

MessageView* MessageStackView::viewAt(int index) const
{
    return qobject_cast<MessageView*>(widget(index));
}

MessageView* MessageStackView::addView(const QString& receiver)
{
    MessageView* view = d.views.value(receiver.toLower());
    if (!view) {
        MessageView::ViewType type = MessageView::ServerView;
        if (!d.views.isEmpty())
            type = session()->isChannel(receiver) ? MessageView::ChannelView : MessageView::QueryView;
        view = createView(type, receiver);
    }
    if (!view->isActive() && d.handler.session()->isChannel(receiver))
        openView(receiver);
    return view;
}

void MessageStackView::restoreView(const ViewInfo& view)
{
    createView(MessageView::ViewType(view.type), view.name);
}

MessageView* MessageStackView::createView(MessageView::ViewType type, const QString& receiver)
{
    MessageView* view = new MessageView(type, d.handler.session(), this);
    // TODO:
    if (IrcSessionInfo(session()).isValid())
        view->completer()->setChannelPrefixes(IrcSessionInfo(session()).channelTypes().join(""));
    view->completer()->setChannelModel(&d.viewModel);
    view->setReceiver(receiver);
    connect(view, SIGNAL(queried(QString)), this, SLOT(addView(QString)));
    connect(view, SIGNAL(queried(QString)), this, SLOT(openView(QString)));
    connect(view, SIGNAL(messaged(QString,QString)), this, SLOT(sendMessage(QString,QString)));

    d.handler.addReceiver(receiver, view);
    d.views.insert(receiver.toLower(), view);
    addWidget(view);
    d.viewModel.setStringList(d.viewModel.stringList() << receiver);
    emit viewAdded(view);
    return view;
}

void MessageStackView::openView(const QString& receiver)
{
    MessageView* view = d.views.value(receiver.toLower());
    if (view)
        setCurrentWidget(view);
}

void MessageStackView::removeView(const QString& receiver)
{
    MessageView* view = d.views.take(receiver.toLower());
    if (view) {
        view->deleteLater();
        QStringList views = d.viewModel.stringList();
        if (views.removeOne(receiver))
            d.viewModel.setStringList(views);
        emit viewRemoved(view);
        d.handler.removeReceiver(view->receiver());
    }
}

void MessageStackView::closeView(int index)
{
    MessageView* view = viewAt(index);
    if (view) {
        if (view->isActive()) {
            if (indexOf(view) == 0)
                session()->quit();
            else if (view->viewType() == MessageView::ChannelView)
                d.handler.session()->sendCommand(IrcCommand::createPart(view->receiver()));
        }
        d.handler.removeReceiver(view->receiver());
    }
}

void MessageStackView::renameView(const QString& from, const QString& to)
{
    MessageView* view = d.views.take(from.toLower());
    if (view) {
        view->setReceiver(to);
        d.views.insert(to.toLower(), view);
        emit viewRenamed(view);
    }
}

void MessageStackView::sendMessage(const QString& receiver, const QString& message)
{
    MessageView* view = addView(receiver);
    if (view) {
        setCurrentWidget(view);
        view->sendMessage(message);
    }
}

void MessageStackView::activateView(int index)
{
    MessageView* view = viewAt(index);
    if (view && isVisible()) {
        d.handler.setCurrentReceiver(view);
        view->setFocus();
        emit viewActivated(view);
    }
}
