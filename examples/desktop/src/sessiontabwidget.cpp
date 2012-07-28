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

#include "sessiontabwidget.h"
#include "application.h"
#include "messageview.h"
#include "settings.h"
#include "session.h"
#include <irccommand.h>
#include <QtGui>

SessionTabWidget::SessionTabWidget(Session* session, QWidget* parent) :
    TabWidget(parent)
{
    d.handler.setSession(session);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
    connect(this, SIGNAL(newTabRequested()), this, SLOT(onNewTabRequested()), Qt::QueuedConnection);
    connect(this, SIGNAL(tabMenuRequested(int,QPoint)), this, SLOT(onTabMenuRequested(int,QPoint)));

    connect(session, SIGNAL(activeChanged(bool)), this, SLOT(updateStatus()));
    connect(session, SIGNAL(connectedChanged(bool)), this, SLOT(updateStatus()));
    connect(session, SIGNAL(networkChanged(QString)), this, SIGNAL(titleChanged(QString)));

    connect(&d.handler, SIGNAL(receiverToBeAdded(QString)), this, SLOT(openView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRemoved(QString)), this, SLOT(removeView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRenamed(QString,QString)), this, SLOT(renameView(QString,QString)));

    QShortcut* shortcut = new QShortcut(QKeySequence::AddTab, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onNewTabRequested()));

    shortcut = new QShortcut(QKeySequence::Close, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(closeCurrentView()));

    applySettings(Application::settings());

    MessageView* view = openView(d.handler.session()->host());
    d.handler.setDefaultReceiver(view);
    updateStatus();
}

Session* SessionTabWidget::session() const
{
    return qobject_cast<Session*>(d.handler.session());
}

MessageView* SessionTabWidget::openView(const QString& receiver)
{
    MessageView* view = d.views.value(receiver.toLower());
    if (!view)
    {
        view = new MessageView(receiver, d.handler.session(), this);
        connect(view, SIGNAL(alert(MessageView*, bool)), this, SLOT(alertTab(MessageView*, bool)));
        connect(view, SIGNAL(highlight(MessageView*, bool)), this, SLOT(highlightTab(MessageView*, bool)));
        connect(view, SIGNAL(query(QString)), this, SLOT(openView(QString)));

        d.handler.addReceiver(receiver, view);
        d.views.insert(receiver.toLower(), view);
        addTab(view, receiver);
    }
    setCurrentWidget(view);
    return view;
}

void SessionTabWidget::removeView(const QString& receiver)
{
    MessageView* view = d.views.take(receiver.toLower());
    if (view)
    {
        view->deleteLater();
        if (indexOf(view) == 0)
        {
            deleteLater();
            session()->destructLater();
        }
    }
}

void SessionTabWidget::closeCurrentView()
{
    closeView(currentIndex());
}

void SessionTabWidget::closeView(int index)
{
    MessageView* view = d.views.value(tabText(index).toLower());
    if (view)
    {
        QString reason = tr("%1 %2").arg(Application::applicationName())
                                    .arg(Application::applicationVersion());
        if (indexOf(view) == 0)
            session()->quit(reason);
        else if (view->isChannelView())
            d.handler.session()->sendCommand(IrcCommand::createPart(view->receiver(), reason));

        d.handler.removeReceiver(view->receiver());
    }
}

void SessionTabWidget::renameView(const QString& from, const QString& to)
{
    MessageView* view = d.views.take(from.toLower());
    if (view)
    {
        view->setReceiver(to);
        d.views.insert(to.toLower(), view);
        int index = indexOf(view);
        if (index != -1)
            setTabText(index, view->receiver());
    }
}

void SessionTabWidget::updateStatus()
{
    bool inactive = !session()->isActive() && !session()->isConnected();
    setTabInactive(0, inactive);
    emit inactiveStatusChanged(inactive);
}

void SessionTabWidget::tabActivated(int index)
{
    if (index < count() - 1)
    {
        d.handler.setCurrentReceiver(qobject_cast<MessageView*>(currentWidget()));
        setTabAlert(index, false);
        setTabHighlight(index, false);
        if (isVisible())
        {
            window()->setWindowFilePath(tabText(index));
            if (currentWidget())
                currentWidget()->setFocus();
        }
    }
}

void SessionTabWidget::onNewTabRequested()
{
    QString channel = QInputDialog::getText(this, tr("Join channel"), tr("Channel:"));
    if (!channel.isEmpty())
    {
        if (session()->isChannel(channel))
            d.handler.session()->sendCommand(IrcCommand::createJoin(channel));
        openView(channel);
    }
}

void SessionTabWidget::onTabMenuRequested(int index, const QPoint& pos)
{
    QMenu menu;
    if (index == 0)
    {
        if (session()->isActive())
            menu.addAction(tr("Disconnect"), session(), SLOT(quit()));
        else
            menu.addAction(tr("Reconnect"), session(), SLOT(reconnect()));
    }
    if (static_cast<MessageView*>(widget(index))->isChannelView())
        menu.addAction(tr("Part"), this, SLOT(onTabCloseRequested()))->setData(index);
    else
        menu.addAction(tr("Close"), this, SLOT(onTabCloseRequested()))->setData(index);
    menu.exec(pos);
}

void SessionTabWidget::onTabCloseRequested()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
        closeView(action->data().toInt());
}

void SessionTabWidget::delayedTabReset()
{
    d.delayedIndexes += currentIndex();
    QTimer::singleShot(500, this, SLOT(delayedTabResetTimeout()));
}

void SessionTabWidget::delayedTabResetTimeout()
{
    if (d.delayedIndexes.isEmpty())
        return;

    int index = d.delayedIndexes.takeFirst();
    tabActivated(index);
}

void SessionTabWidget::alertTab(MessageView* view, bool on)
{
    int index = indexOf(view);
    if (index != -1)
    {
        if (!isVisible() || !isActiveWindow() || index != currentIndex())
            setTabAlert(index, on);
    }
}

void SessionTabWidget::highlightTab(MessageView* view, bool on)
{
    int index = indexOf(view);
    if (index != -1)
    {
        if (!isVisible() || !isActiveWindow() || index != currentIndex())
            setTabHighlight(index, on);
    }
}

void SessionTabWidget::applySettings(const Settings& settings)
{
    setAlertColor(QColor(settings.colors.value(Settings::Highlight)));
    setHighlightColor(QColor(settings.colors.value(Settings::Highlight)));
}
