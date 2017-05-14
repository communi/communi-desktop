/*
  Copyright (C) 2008-2017 The Communi Project

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

#include "loggerplugin.h"
#include <IrcConnection>
#include <IrcNetwork>
#include <IrcMessage>
#include <IrcCommand>
#include <IrcChannel>
#include <IrcConnection>
#include <IrcBufferModel>
#include <Irc>
#include <QDir>
#include <QTextStream>
#include <QSettings>
#include <QDebug>

LoggerPlugin::LoggerPlugin(QObject* parent) : QObject(parent)
    , m_connections(0)
{
    this->settingsChanged();
}

LoggerPlugin::~LoggerPlugin()
{
    // Iterate over all log items and close all open files, etc.
    foreach (IrcBuffer *buf, this->m_logitems.keys()) {
        this->removeLogitemForBuffer(buf);
    }
}

void LoggerPlugin::setConnectionsList(const QList<IrcConnection*>* list)
{
    if (list != this->m_connections) {
        pluginDisabled();
        this->m_connections = list;
        pluginEnabled();
    }
}

void LoggerPlugin::pluginEnabled()
{
    if (!this->m_connections)
        return;

    foreach (IrcConnection* conn, *(this->m_connections)) {
        IrcBufferModel* model = conn->findChild<IrcBufferModel*>();
        foreach (IrcBuffer* buf, model->buffers()) {
            this->bufferAdded(buf);
        }
    }
}

void LoggerPlugin::pluginDisabled()
{
    foreach (IrcBuffer *buf, this->m_logitems.keys()) {
        this->bufferRemoved(buf);
    }
}

void LoggerPlugin::bufferAdded(IrcBuffer* buffer)
{
    // Do not log connection buffers and #magna
    if (buffer->network()->name().isEmpty())
        return;

    // Don't add buffer if already added
    if (this->m_logitems.contains(buffer))
        return;

    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(logMessage(IrcMessage*)));
    connect(buffer, SIGNAL(destroyed()), this, SLOT(onBufferDestroyed()));

    const QString filename = logfileName(buffer);
    Item item;
    item.logfile = new QFile(m_logDirPath + "/" + filename, this);
    item.logfile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    item.textStream = new QTextStream(item.logfile);

    this->m_logitems.insert(buffer, item);
    writeToFile(buffer, "=== Logfile started on " + timestamp() + " ===");
}

void LoggerPlugin::bufferRemoved(IrcBuffer* buffer)
{
    disconnect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(logMessage(IrcMessage*)));
    disconnect(buffer, SIGNAL(destroyed()), this, SLOT(onBufferDestroyed()));

    removeLogitemForBuffer(buffer);
}

void LoggerPlugin::removeLogitemForBuffer(IrcBuffer *buffer) {
    if (this->m_logitems.contains(buffer)) {
        Item item = this->m_logitems.take(buffer);

        item.textStream->flush();
        delete item.textStream;

        item.logfile->close();
        delete item.logfile;
    }
}

void LoggerPlugin::settingsChanged()
{
    QSettings settings;
    QString loggingLocation = settings.value("loggingLocation").toString();

    if (m_logDirPath != loggingLocation) {
        pluginDisabled();

        m_logDirPath = loggingLocation;
        QDir logDir;
        if (!logDir.exists(m_logDirPath))
            logDir.mkpath(m_logDirPath);

        pluginEnabled();
    }
}

void LoggerPlugin::logMessage(IrcMessage *message)
{
    if (message->type() != IrcMessage::Private)
        return;

    IrcBuffer *buffer = qobject_cast<IrcBuffer*>(QObject::sender());

    if (buffer) {
        IrcPrivateMessage *m = static_cast<IrcPrivateMessage*>(message);
        writeToFile(buffer, timestamp() + " " + m->nick() + ": " + m->content());
    }
}

void LoggerPlugin::writeToFile(IrcBuffer* buffer, const QString &text)
{
    Item item = this->m_logitems[buffer];
    *(item.textStream) << text << endl;
}

QString LoggerPlugin::logfileName(IrcBuffer *buffer) const
{
    return buffer->network()->name() + "_" + buffer->title() + ".log";
}

QString LoggerPlugin::timestamp() const
{
    return QDateTime::currentDateTime().toString("[yyyy-MM-dd] hh:mm:ss");
}
