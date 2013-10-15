/*
* Copyright (C) 2008-2013 The Communi Project
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ignoremanager.h"
#include <ircconnection.h>
#include <ircmessage.h>
#include <qregexp.h>
#include <irc.h>

IgnoreManager* IgnoreManager::instance()
{
    static IgnoreManager manager;
    return &manager;
}

IgnoreManager::IgnoreManager(QObject* parent) : QObject(parent)
{
}

IgnoreManager::~IgnoreManager()
{
}

bool IgnoreManager::messageFilter(IrcMessage* message)
{
    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        foreach (const QString& ignore, d.ignores) {
            QRegExp rx(ignore, Qt::CaseInsensitive, QRegExp::Wildcard);
            if (rx.exactMatch(message->prefix()))
                return true;
        }
    }
    return false;
}

QStringList IgnoreManager::ignores() const
{
    return d.ignores;
}

static QString masked(const QString& ignore)
{
    QString nick = Irc::nickFromPrefix(ignore);
    QString ident = Irc::identFromPrefix(ignore);
    QString host = Irc::hostFromPrefix(ignore);
    if (nick.isEmpty())
        nick = ignore;
    if (ident.isEmpty())
        ident = "*";
    if (host.isEmpty())
        host = "*";
    return nick + "!" + ident + "@" + host;
}

QString IgnoreManager::addIgnore(const QString& ignore)
{
    const QString mask = masked(ignore);
    if (!d.ignores.contains(mask))
        d.ignores.append(mask);
    return mask;
}

QString IgnoreManager::removeIgnore(const QString& ignore)
{
    const QString mask = masked(ignore);
    d.ignores.removeAll(mask);
    return mask;
}

void IgnoreManager::setIgnores(const QStringList& ignores)
{
    d.ignores.clear();
    foreach (const QString& ignore, ignores)
        d.ignores.append(masked(ignore));
}

void IgnoreManager::addConnection(IrcConnection* connection)
{
    connection->installMessageFilter(this);
}

void IgnoreManager::removeConnection(IrcConnection* connection)
{
    connection->removeMessageFilter(this);
}
