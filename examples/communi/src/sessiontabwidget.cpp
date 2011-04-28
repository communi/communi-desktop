/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#include "sessiontabwidget.h"
#include "application.h"
#include "messageview.h"
#include "settings.h"
#include "session.h"
#include <ircmessage.h>
#include <ircprefix.h>
#include <ircutil.h>
#include <irc.h>
#include <QtGui>

SessionTabWidget::SessionTabWidget(Session* session, QWidget* parent) :
    TabWidget(parent)
{
    d.connectCounter = 0;
    d.session = session;

    // take ownership of the session
    session->setParent(this);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
    connect(session, SIGNAL(connected()), this, SLOT(connected()));
    connect(session, SIGNAL(connecting()), this, SLOT(connecting()));
    connect(session, SIGNAL(disconnected()), this, SLOT(disconnected()));

    QShortcut* shortcut = new QShortcut(QKeySequence::Close, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(closeView()));

    applySettings(Application::settings());

#if QT_VERSION >= 0x040600
    registerSwipeGestures(Qt::Horizontal);
#endif

    openView(session->host());
    connect(session, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

Session* SessionTabWidget::session() const
{
    return d.session;
}

void SessionTabWidget::openView(const QString& receiver)
{
    MessageView* view = d.views.value(receiver.toLower());
    if (!view)
    {
        view = new MessageView(d.session, this);
        view->setReceiver(receiver);
        connect(view, SIGNAL(rename(MessageView*)), this, SLOT(nameTab(MessageView*)));
        connect(view, SIGNAL(alert(MessageView*, bool)), this, SLOT(alertTab(MessageView*, bool)));
        connect(view, SIGNAL(highlight(MessageView*, bool)), this, SLOT(highlightTab(MessageView*, bool)));

        d.views.insert(receiver.toLower(), view);
        addTab(view, receiver);
        setCurrentWidget(view);
    }
}

void SessionTabWidget::closeView(int index)
{
    if (index == -1)
        index = currentIndex();

    if (index != -1)
    {
        MessageView* view = d.views.take(tabText(index).toLower());
        if (view)
        {
            if (indexOf(view) == 0)
            {
                // closing a server tab
                emit disconnectFrom(QString());
                deleteLater();
            }
            view->deleteLater();
        }
    }
}

void SessionTabWidget::connected()
{
    MessageView* view = static_cast<MessageView*>(widget(0));
    if (view)
    {
        QStringList params;
        params << d.session->host() << tr("Connected");
        view->receiveMessage(tr("[%1] %2"), params);
        highlightTab(view, true);
    }
    d.connectCounter = 0;
}

void SessionTabWidget::connecting()
{
    MessageView* view = static_cast<MessageView*>(widget(0));
    if (view)
    {
        QStringList params;
        params << d.session->host() << tr("Connecting... #%1").arg(++d.connectCounter);
        view->receiveMessage(tr("[%1] %2"), params);
        highlightTab(view, true);
    }
}

void SessionTabWidget::disconnected()
{
    MessageView* view = static_cast<MessageView*>(widget(0));
    if (view)
    {
        QStringList params;
        params << d.session->host() << tr("Disconnected");
        view->receiveMessage(tr("[%1] %2"), params);
        highlightTab(view, true);
    }
}

void SessionTabWidget::tabActivated(int index)
{
    setTabAlert(index, false);
    setTabHighlight(index, false);
    if (isVisible())
    {
        window()->setWindowFilePath(tabText(index));
        if (currentWidget())
            currentWidget()->setFocus();
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

void SessionTabWidget::nameTab(MessageView* view)
{
    int index = indexOf(view);
    if (index != -1)
    {
        d.views.remove(tabText(index));
        d.views.insert(view->receiver(), view);

        setTabText(index, view->receiver());
        if (index == 0)
            emit titleChanged(view->receiver());
    }
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

void SessionTabWidget::onMessageReceived(IrcMessage* message)
{
    if (IrcChannelMessage* channelMessage = qobject_cast<IrcChannelMessage*>(message))
    {
        openView(channelMessage->channel());
    }
    else if (IrcSendMessage* sendMessage = qobject_cast<IrcSendMessage*>(message))
    {
        if (sendMessage->target().toLower() == session()->nickName().toLower())
            openView(IrcPrefix(sendMessage->prefix()).nick());
    }
}
