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
#include <Irc>
#include <QDir>
#include <QTextStream>
#include <QSettings>
#include <QDebug>

LoggerPlugin::LoggerPlugin(QObject* parent) : QObject(parent)
{
    this->settingsChanged();
}

LoggerPlugin::~LoggerPlugin()
{
}

void LoggerPlugin::bufferAdded(IrcBuffer* buffer)
{
    // Do not log connection buffers and #magna
    if (buffer->network()->name().isEmpty())
        return;

    const QString filename = logfileName(buffer);
    writeToFile(filename, "=== Logfile started on " + timestamp() + " ===");

    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(logMessage(IrcMessage*)));
}

void LoggerPlugin::bufferRemoved(IrcBuffer* buffer)
{
    disconnect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(logMessage(IrcMessage*)));
}

void LoggerPlugin::settingsChanged()
{
    QSettings settings;
    m_logDirPath = settings.value("loggingLocation").toString();
    QDir logDir;
    if (!logDir.exists(m_logDirPath))
        logDir.mkpath(m_logDirPath);
}

void LoggerPlugin::logMessage(IrcMessage *message)
{
    if (message->type() != IrcMessage::Private)
        return;

    IrcPrivateMessage *m = static_cast<IrcPrivateMessage*>(message);
    const QString filename = logfileName(m);
    writeToFile(filename , timestamp() + " " + m->nick() + ": " + m->content());
}

void LoggerPlugin::writeToFile(const QString &fileName, const QString &text)
{
    QFile logfile(m_logDirPath + "/" + fileName);
    logfile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);

    QTextStream ts(&logfile);
    ts << text << endl;
}

QString LoggerPlugin::logfileName(IrcPrivateMessage *message) const
{
    return message->isPrivate() ? message->network()->name() + "_" + message->nick() + ".log"
                                : message->network()->name() + "_" + message->target() + ".log";
}

QString LoggerPlugin::logfileName(IrcBuffer *buffer) const
{
    return buffer->network()->name() + "_" + buffer->title() + ".log";
}

QString LoggerPlugin::timestamp() const
{
    return QDateTime::currentDateTime().toString("[yyyy-MM-dd] hh:mm:ss");
}
