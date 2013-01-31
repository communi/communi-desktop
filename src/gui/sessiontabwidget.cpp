/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
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
#include "addviewdialog.h"
#include "messageview.h"
#include "session.h"
#include <irccommand.h>
#include <QShortcut>

SessionTabWidget::SessionTabWidget(Session* session, QWidget* parent) : QStackedWidget(parent)
{
    d.handler.setSession(session);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));

    connect(&d.handler, SIGNAL(receiverToBeAdded(QString)), this, SLOT(openView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRemoved(QString)), this, SLOT(removeView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRenamed(QString, QString)), this, SLOT(renameView(QString, QString)));

    QShortcut* shortcut = new QShortcut(QKeySequence::AddTab, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onNewTabRequested()));

    shortcut = new QShortcut(QKeySequence::Close, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(closeCurrentView()));

    MessageView* view = openView(d.handler.session()->host());
    d.handler.setDefaultReceiver(view);
    d.handler.setCurrentReceiver(view);
}

Session* SessionTabWidget::session() const
{
    return qobject_cast<Session*>(d.handler.session());
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

MessageView* SessionTabWidget::openView(const QString& receiver)
{
    MessageView* view = d.views.value(receiver.toLower());
    if (!view) {
        MessageView::ViewType type = MessageView::ServerView;
        if (!d.views.isEmpty())
            type = session()->isChannel(receiver) ? MessageView::ChannelView : MessageView::QueryView;
        view = new MessageView(type, d.handler.session(), this);
        view->setReceiver(receiver);
        connect(view, SIGNAL(alerted(IrcMessage*)), this, SLOT(onTabAlerted(IrcMessage*)));
        connect(view, SIGNAL(highlighted(IrcMessage*)), this, SLOT(onTabHighlighted(IrcMessage*)));
        connect(view, SIGNAL(queried(QString)), this, SLOT(openView(QString)));
        connect(view, SIGNAL(splitterChanged(QByteArray)), this, SLOT(restoreSplitter(QByteArray)));

        d.handler.addReceiver(receiver, view);
        d.views.insert(receiver.toLower(), view);
        addWidget(view);
        emit viewAdded(view);
    }
    setCurrentWidget(view);
    return view;
}

void SessionTabWidget::removeView(const QString& receiver)
{
    MessageView* view = d.views.take(receiver.toLower());
    if (view) {
        view->deleteLater();
        emit viewRemoved(view);
        if (indexOf(view) == 0) {
            deleteLater();
            session()->destructLater();
            emit sessionClosed(session());
        }

        d.handler.removeReceiver(view->receiver());
    }
}

void SessionTabWidget::closeCurrentView()
{
    closeView(currentIndex());
}

void SessionTabWidget::closeView(int index)
{
    MessageView* view = viewAt(index);
    if (view) {
        if (view->isActive()) {
            QString reason = tr("%1 %2").arg(QApplication::applicationName())
                             .arg(QApplication::applicationVersion());
            if (indexOf(view) == 0)
                session()->quit(reason);
            else if (view->viewType() == MessageView::ChannelView)
                d.handler.session()->sendCommand(IrcCommand::createPart(view->receiver(), reason));
        }
        d.handler.removeReceiver(view->receiver());
    }
}

void SessionTabWidget::renameView(const QString& from, const QString& to)
{
    MessageView* view = d.views.take(from.toLower());
    if (view) {
        view->setReceiver(to);
        d.views.insert(to.toLower(), view);
        emit viewRenamed(view);
    }
}

void SessionTabWidget::tabActivated(int index)
{
    MessageView* view = viewAt(index);
    if (view && isVisible()) {
        d.handler.setCurrentReceiver(view);
        view->setFocus();
        emit viewActivated(view);
    }
}

void SessionTabWidget::onNewTabRequested()
{
    AddViewDialog dialog(session(), this);
    if (dialog.exec()) {
        QString view = dialog.view();
        if (session()->isChannel(view))
            d.handler.session()->sendCommand(IrcCommand::createJoin(view, dialog.password()));
        openView(view);
    }
}

void SessionTabWidget::onTabAlerted(IrcMessage* message)
{
    MessageView* view = qobject_cast<MessageView*>(sender());
    int index = indexOf(view);
    if (index != -1) {
        if (!isVisible() || !isActiveWindow() || index != currentIndex())
            emit alerted(view, message);
    }
}

void SessionTabWidget::onTabHighlighted(IrcMessage* message)
{
    MessageView* view = qobject_cast<MessageView*>(sender());
    int index = indexOf(view);
    if (index != -1) {
        if (!isVisible() || !isActiveWindow() || index != currentIndex())
            emit highlighted(view, message);
    }
}
