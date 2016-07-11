/*
  Copyright (C) 2016 Tor Arne Vestbø

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "messageseenplugin.h"

#include "bufferview.h"
#include "textdocument.h"

#include <IrcConnection>
#include <IrcBufferModel>
#include <IrcBuffer>
#include <IrcCommand>

#include <QtCore/qscopedvaluerollback.h>
#include <QtCore/qcoreevent.h>

static const char* kMessageSeenCapability = "znc.in/message-seen";
static const int kMessageCompressionDelay = 200;

MessageSeenPlugin::MessageSeenPlugin(QObject* parent) : QObject(parent)
{
}

void MessageSeenPlugin::connectionAdded(IrcConnection* connection)
{
    IrcNetwork* network = connection->network();
    QStringList capabilities = network->requestedCapabilities();
    capabilities += kMessageSeenCapability;
    network->setRequestedCapabilities(capabilities);

    connection->installMessageFilter(this);
}

void MessageSeenPlugin::documentAdded(TextDocument *document)
{
    connect(document, &TextDocument::latestMessageSeenChanged, this, &MessageSeenPlugin::latestMessageSeenChanged);
}

class IrcMessageSeenCommand : public IrcCommand
{
    Q_OBJECT
    explicit IrcMessageSeenCommand(QObject* parent = 0) : IrcCommand(parent) {}

public:
    static IrcCommand* create(const QString& target, const QDateTime &timeStamp)
    {
        IrcCommand* command = new IrcMessageSeenCommand;
        command->setParameters(QStringList() << target << timeStamp.toUTC().toString(Qt::ISODate));
        return command;
    }

    QString toString() const Q_DECL_OVERRIDE
    {
        return QString("MSGSEEN %1 %2").arg(parameters().value(0), parameters().value(1));
    }
};

void MessageSeenPlugin::latestMessageSeenChanged(const QDateTime& timestamp)
{
    Q_UNUSED(timestamp);

    // FIXME: This signal is also emitted when a synthetic message is added
    // to the document without going through the server, in which case the
    // timestamp does not represent server time. We need a way to detect this
    // and skip sending MSGSEEN for those messages.

    if (m_processingMsgSeenMessage)
        return;

    TextDocument *document = qobject_cast<TextDocument *>(sender());
    if (document->isClone())
        return;

    IrcBuffer* buffer = document->buffer();
    if (buffer->isSticky()) {
        // Skip server messages, as there's no shared target name between
        // clients for this buffer. FIXME: Use more explicit flag.
        return;
    }

    if (!buffer->network()->isCapable(kMessageSeenCapability))
        return;

    if (int timerId = m_sendTimers.take(buffer))
        killTimer(timerId);

    m_sendTimers.insert(buffer, startTimer(kMessageCompressionDelay));
}

void MessageSeenPlugin::timerEvent(QTimerEvent* event)
{
    IrcBuffer* buffer = m_sendTimers.key(event->timerId(), 0);
    if (!buffer)
        return;

    foreach (TextDocument* document, buffer->findChildren<TextDocument*>()) {
        if (document->isClone())
            continue;

        QDateTime timestamp = document->latestMessageSeen();
        IrcCommand *command = IrcMessageSeenCommand::create(buffer->title(), timestamp);

        command->setParent(this);
        buffer->sendCommand(command);
        command->deleteLater();

        break;
    }

    killTimer(event->timerId());
    m_sendTimers.remove(buffer);
}

bool MessageSeenPlugin::messageFilter(IrcMessage* message)
{
    if (message->command() != QStringLiteral("MSGSEEN"))
        return false;

    if (!message->isOwn()) {
        // A message was seen by another user in the conversation. Right now
        // this is not supported by any servers, so we don't do anything here.
        return true;
    }

    QScopedValueRollback<bool> rollback(m_processingMsgSeenMessage, true);

    QString title = message->parameters().at(0);
    QDateTime timestamp = QDateTime::fromString(message->parameters().at(1), Qt::ISODate);
    if (!timestamp.isValid())
        return true;

    IrcConnection* connection = message->connection();
    foreach (IrcBufferModel* bufferModel, connection->findChildren<IrcBufferModel*>()) {
        IrcBuffer *buffer = bufferModel->find(title);
        if (!buffer)
            continue;

        foreach (TextDocument* document, buffer->findChildren<TextDocument*>()) {
            QDateTime previousLastSeenTimestamp = document->latestMessageSeen();
            if (timestamp > previousLastSeenTimestamp)
                document->setLatestMessageSeen(timestamp);
        }
    }

    return true;
}

#include "messageseenplugin.moc"
