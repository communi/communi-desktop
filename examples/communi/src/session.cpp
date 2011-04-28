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

#include "session.h"
#include "application.h"
#ifndef QT_NO_OPENSSL
#include <QSslSocket>
#endif // QT_NO_OPENSSL
#include <ircmessage.h>

Session::Session(QObject* parent) : IrcSession(parent), delay(10)
{
    connect(this, SIGNAL(password(QString*)), SLOT(onPassword(QString*)));
    //TODO: setOptions(options() | Session::PrefixNicks);
    Application::setSessions(Application::sessions() << this);
}

Session::~Session()
{
    QList<Session*> sessions = Application::sessions();
    if (sessions.removeOne(this))
        Application::setSessions(sessions);
}

Connection Session::connection() const
{
    conn.channels.clear();
    /* TODO:
    QList<Irc::Buffer*> buffers = findChildren<Irc::Buffer*>();
    foreach (Irc::Buffer* buffer, buffers)
    {
        QString receiver = buffer->receiver();
        if (isChannel(receiver))
            conn.channels += receiver;
    }
    */
    return conn;
}

QStringList Session::autoJoinChannels() const
{
    return channels;
}

void Session::setAutoJoinChannels(const QStringList& arg)
{
    channels = arg;
}

int Session::autoReconnectDelay() const
{
    return delay;
}

void Session::setAutoReconnectDelay(int seconds)
{
    delay = seconds;
}

void Session::connectTo(const Connection& connection)
{
#ifndef QT_NO_OPENSSL
    if (connection.secure)
    {
        QSslSocket* socket = new QSslSocket(this);
        socket->setPeerVerifyMode(QSslSocket::VerifyNone);
        socket->ignoreSslErrors();
        setSocket(socket);
    }
#endif // QT_NO_OPENSSL
    
    QString appName = QApplication::applicationName();

    setHost(connection.host);
    setPort(connection.port);
    setNickName(connection.nick);
    setUserName(appName.toLower());
    setRealName(connection.real.isEmpty() ? appName : connection.real);
    setAutoJoinChannels(connection.channels);
    open();

    conn = connection;
}

void Session::onPassword(QString* password)
{
    *password = conn.pass;
}

void Session::onMessageReceived(IrcMessage* message)
{
    if (message->type() == Irc::RPL_ENDOFMOTD || message->type() == Irc::ERR_NOMOTD)
    {
        foreach (const QString& channel, channels)
        {
            IrcJoinMessage msg;
            msg.setChannel(channel);
            sendMessage(&msg);
        }
    }
}
