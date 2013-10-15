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

#include "zncmanager.h"
#include "ignoremanager.h"
#include <ircconnection.h>
#include <ircmessage.h>
#include <ircbuffer.h>

ZncManager::ZncManager(QObject* parent) : QObject(parent)
{
    d.model = 0;
    d.buffer = 0;
    d.timestamp = 0;
    d.playback = false;
    d.timestamper.invalidate();
    d.timeStampFormat = "[hh:mm:ss]";
    setModel(qobject_cast<IrcBufferModel*>(parent));
}

ZncManager::~ZncManager()
{
}

bool ZncManager::isPlaybackActive() const
{
    return d.playback;
}

QString ZncManager::playbackTarget() const
{
    return d.target;
}

IrcBufferModel* ZncManager::model() const
{
    return d.model;
}

void ZncManager::setModel(IrcBufferModel* model)
{
    if (d.model != model) {
        if (d.model && d.model->connection()) {
            IrcConnection* connection = d.model->connection();
            disconnect(connection, SIGNAL(connected()), this, SLOT(onConnected()));
            disconnect(connection->network(), SIGNAL(requestingCapabilities()), this, SLOT(requestCapabilities()));
            connection->removeMessageFilter(this);
        }
        d.model = model;
        if (d.model && d.model->connection()) {
            IrcConnection* connection = d.model->connection();
            connect(connection, SIGNAL(connected()), this, SLOT(onConnected()));
            connect(connection->network(), SIGNAL(requestingCapabilities()), this, SLOT(requestCapabilities()));
            connection->installMessageFilter(this);
        }
        emit modelChanged(model);
    }
}

QString ZncManager::timeStampFormat() const
{
    return d.timeStampFormat;
}

void ZncManager::setTimeStampFormat(const QString& format)
{
    if (d.timeStampFormat != format) {
        d.timeStampFormat = format;
        emit timeStampFormatChanged(format);
    }
}

bool ZncManager::messageFilter(IrcMessage* message)
{
    if (d.timestamp > 0 && d.timestamper.isValid()) {
        long elapsed = d.timestamper.elapsed() / 1000;
        if (elapsed > 0) {
            d.timestamp += elapsed;
            d.timestamper.restart();
        }
    }

    if (message->type() == IrcMessage::Private) {
        if (message->nick() == QLatin1String("***") && message->ident() == QLatin1String("znc")) {
            IrcPrivateMessage* privMsg = static_cast<IrcPrivateMessage*>(message);
            QString content = privMsg->content();
            if (content == QLatin1String("Buffer Playback...")) {
                if (!d.playback) {
                    d.playback = true;
                    emit playbackActiveChanged(d.playback);
                }
                if (d.target != privMsg->target()) {
                    d.target = privMsg->target();
                    d.buffer = d.model->find(d.target);
                    emit playbackTargetChanged(d.target);
                }
                return false;
            } else if (content == QLatin1String("Playback Complete.")) {
                if (d.playback) {
                    d.playback = false;
                    emit playbackActiveChanged(d.playback);
                }
                if (!d.target.isEmpty()) {
                    d.buffer = 0;
                    d.target.clear();
                    emit playbackTargetChanged(d.target);
                }
                return false;
            }
        }
    } else if (message->type() == IrcMessage::Notice) {
        if (message->nick() == "*communi") {
            d.timestamp = static_cast<IrcNoticeMessage*>(message)->content().toLong();
            d.timestamper.restart();
            return true;
        }
    }

    if (d.playback && d.buffer) {
        switch (message->type()) {
        case IrcMessage::Private:
            return processMessage(static_cast<IrcPrivateMessage*>(message));
        case IrcMessage::Notice:
            return processNotice(static_cast<IrcNoticeMessage*>(message));
        default:
            break;
        }
    }
    return false;
}

bool ZncManager::processMessage(IrcPrivateMessage* message)
{
    QString msg = message->content();
    int idx = msg.indexOf(" ");
    if (idx != -1) {
        QDateTime timeStamp = QDateTime::fromString(msg.left(idx), d.timeStampFormat);
        if (timeStamp.isValid()) {
            msg.remove(0, idx + 1);

            if (message->nick() == "*buffextras") {
                idx = msg.indexOf(" ");
                QString prefix = msg.left(idx);
                QString content = msg.mid(idx + 1);

                IrcMessage* tmp = 0;
                if (content.startsWith("joined")) {
                    tmp = IrcMessage::fromParameters(prefix, "JOIN", QStringList() << message->target(), message->connection());
                } else if (content.startsWith("parted")) {
                    QString reason = content.mid(content.indexOf("[") + 1);
                    reason.chop(1);
                    tmp = IrcMessage::fromParameters(prefix, "PART", QStringList() << message->target() << reason , message->connection());
                } else if (content.startsWith("quit")) {
                    QString reason = content.mid(content.indexOf("[") + 1);
                    reason.chop(1);
                    tmp = IrcMessage::fromParameters(prefix, "QUIT", QStringList() << reason , message->connection());
                } else if (content.startsWith("is")) {
                    QStringList tokens = content.split(" ", QString::SkipEmptyParts);
                    tmp = IrcMessage::fromParameters(prefix, "NICK", QStringList() << tokens.last() , message->connection());
                } else if (content.startsWith("set")) {
                    QStringList tokens = content.split(" ", QString::SkipEmptyParts);
                    QString user = tokens.takeLast();
                    QString mode = tokens.takeLast();
                    tmp = IrcMessage::fromParameters(prefix, "MODE", QStringList() << message->target() << mode << user, message->connection());
                } else if (content.startsWith("changed")) {
                    QString topic = content.mid(content.indexOf(":") + 2);
                    tmp = IrcMessage::fromParameters(prefix, "TOPIC", QStringList() << message->target() << topic, message->connection());
                } else if (content.startsWith("kicked")) {
                    QString reason = content.mid(content.indexOf("[") + 1);
                    reason.chop(1);
                    QStringList tokens = content.split(" ", QString::SkipEmptyParts);
                    tmp = IrcMessage::fromParameters(prefix, "KICK", QStringList() << message->target() << tokens.value(1) << reason, message->connection());
                }
                if (tmp) {
                    tmp->setTimeStamp(timeStamp);
                    d.buffer->receiveMessage(tmp);
                    tmp->deleteLater();
                    return true;
                }
            }

            if (message->isAction())
                msg = QString("\1ACTION %1\1").arg(msg);
            else if (message->isRequest())
                msg = QString("\1%1\1").arg(msg);
            message->setParameters(QStringList() << message->target() << msg);
            message->setTimeStamp(timeStamp);
        }
    }
    return IgnoreManager::instance()->messageFilter(message);
}

bool ZncManager::processNotice(IrcNoticeMessage* message)
{
    QString msg = message->content();
    int idx = msg.indexOf(" ");
    if (idx != -1) {
        QDateTime timeStamp = QDateTime::fromString(msg.left(idx), d.timeStampFormat);
        if (timeStamp.isValid()) {
            message->setTimeStamp(timeStamp);
            msg.remove(0, idx + 1);
            if (message->isReply())
                msg = QString("\1%1\1").arg(msg);
            message->setParameters(QStringList() << message->target() << msg);
        }
    }
    return IgnoreManager::instance()->messageFilter(message);
}

void ZncManager::onConnected()
{
    d.timestamper.invalidate();
}

void ZncManager::requestCapabilities()
{
    QStringList available = d.model->network()->availableCapabilities();
    if (available.contains("communi")) {
        QStringList caps = QStringList() << "communi" << QString("communi/%1").arg(d.timestamp);
        d.model->network()->requestCapabilities(caps);
    }
}
