/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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
    d.hasQuit = false;
    d.handler.setSession(session);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
    connect(this, SIGNAL(newTabRequested()), this, SLOT(onNewTabRequested()));
    connect(session, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

    connect(&d.handler, SIGNAL(receiverToBeAdded(QString)), this, SLOT(openView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRemoved(QString)), this, SLOT(removeView(QString)));
    connect(&d.handler, SIGNAL(receiverToBeRenamed(QString,QString)), this, SLOT(renameView(QString,QString)));

    QShortcut* shortcut = new QShortcut(QKeySequence::Close, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(closeCurrentView()));

    applySettings(Application::settings());

#if QT_VERSION >= 0x040600
    registerSwipeGestures(Qt::Horizontal);
#endif

    MessageView* view = openView(d.handler.session()->host());
    d.handler.setDefaultReceiver(view);
}

Session* SessionTabWidget::session() const
{
    return qobject_cast<Session*>(d.handler.session());
}

QStringList SessionTabWidget::channels() const
{
    QStringList chans;
    foreach (MessageView* view, d.views)
    {
        if (view->isChannelView())
            chans += view->receiver();
    }
    return chans;
}

MessageView* SessionTabWidget::openView(const QString& receiver)
{
    MessageView* view = d.views.value(receiver.toLower());
    if (!view)
    {
        view = new MessageView(d.handler.session(), this);
        view->setReceiver(receiver);
        connect(view, SIGNAL(alert(MessageView*, bool)), this, SLOT(alertTab(MessageView*, bool)));
        connect(view, SIGNAL(highlight(MessageView*, bool)), this, SLOT(highlightTab(MessageView*, bool)));
        connect(view, SIGNAL(query(QString)), this, SLOT(openView(QString)));
        connect(view, SIGNAL(aboutToQuit()), this, SLOT(onAboutToQuit()));

        d.handler.addReceiver(receiver, view);
        d.views.insert(receiver.toLower(), view);
        addTab(view, receiver);
        setCurrentWidget(view);
    }
    return view;
}

void SessionTabWidget::removeView(const QString& receiver)
{
    MessageView* view = d.views.take(receiver.toLower());
    if (view)
    {
        if (indexOf(view) == 0)
            deleteLater();
        else
            view->deleteLater();
    }
}

void SessionTabWidget::closeCurrentView()
{
    MessageView* view = d.views.value(tabText(currentIndex()).toLower());
    if (view)
    {
        if (indexOf(view) == 0)
            quit();
        else if (view->isChannelView())
            d.handler.session()->sendCommand(IrcCommand::createPart(view->receiver()));

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
        if (index == 0)
            emit titleChanged(view->receiver());
    }
}

void SessionTabWidget::quit(const QString &message)
{
    QString reason = message.trimmed();
    if (reason.isEmpty())
        reason = tr("%1 %2").arg(Application::applicationName())
                            .arg(Application::applicationVersion());
    d.handler.session()->sendCommand(IrcCommand::createQuit(reason));
}

void SessionTabWidget::onAboutToQuit()
{
    d.hasQuit = true;
}

void SessionTabWidget::onDisconnected()
{
    if (d.hasQuit)
        deleteLater();
    // TODO: else reconnect?
}

void SessionTabWidget::tabActivated(int index)
{
    if (index < count() - 1)
    {
        d.handler.setCurrentReceiver(currentWidget());
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
        if (channel.startsWith("#") || channel.startsWith("&"))
            d.handler.session()->sendCommand(IrcCommand::createJoin(channel));
        openView(channel);
    }
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
        emit vibraRequested(on);
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
