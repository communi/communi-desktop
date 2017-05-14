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

#ifndef LOGGERPLUGIN_H
#define LOGGERPLUGIN_H

#include <QtPlugin>
#include <QMap>
#include <IrcMessageFilter>
#include "bufferplugin.h"
#include "settingsplugin.h"
#include "connectionplugin.h"
#include "genericplugin.h"

class QFile;
class QTextStream;

class IrcChannel;
class IrcPrivateMessage;

class LoggerPlugin : public QObject, public BufferPlugin, public SettingsPlugin, public ConnectionPlugin, public GenericPlugin
{
    Q_OBJECT
    Q_INTERFACES(BufferPlugin SettingsPlugin ConnectionPlugin GenericPlugin)
    Q_PLUGIN_METADATA(IID "Communi.BufferPlugin")
    Q_PLUGIN_METADATA(IID "Communi.SettingsPlugin")
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")
    Q_PLUGIN_METADATA(IID "Communi.GenericPlugin")

    struct Item
    {
        QFile* logfile;
        QTextStream* textStream;
    };

public:
    LoggerPlugin(QObject* parent = 0);
    ~LoggerPlugin();
    void bufferAdded(IrcBuffer* buffer);
    void bufferRemoved(IrcBuffer* buffer);
    void settingsChanged();
    void setConnectionsList(const QList<IrcConnection*>* list);
    void pluginEnabled();
    void pluginDisabled();

private slots:
    void logMessage(IrcMessage *message);
    void removeLogitemForBuffer(IrcBuffer *buffer);

private:
    void writeToFile(IrcBuffer* buffer, const QString &text);
    QString logfileName(IrcBuffer *buffer) const;
    QString timestamp() const;

    QString m_logDirPath;
    QMap<IrcBuffer*, Item> m_logitems;
    const QList<IrcConnection*>* m_connections;
};

#endif // LOGGERPLUGIN_H
