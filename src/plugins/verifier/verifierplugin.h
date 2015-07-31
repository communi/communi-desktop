/*
  Copyright (C) 2008-2015 The Communi Project

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

#ifndef VERIFIERPLUGIN_H
#define VERIFIERPLUGIN_H

#include <QHash>
#include <QtPlugin>
#include <QMultiHash>
#include "connectionplugin.h"
#include "documentplugin.h"

class IrcCommand;
class IrcMessage;
class CommandVerifier;
class QTextDocument;

class VerifierPlugin : public QObject, public ConnectionPlugin, public DocumentPlugin
{
    Q_OBJECT
    Q_INTERFACES(ConnectionPlugin DocumentPlugin)
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")
    Q_PLUGIN_METADATA(IID "Communi.DocumentPlugin")

public:
    VerifierPlugin(QObject* parent = 0);

    void connectionAdded(IrcConnection* connection);
    void documentAdded(TextDocument* document);

private slots:
    void onCommandVerified(int id);
    void onMessageReceived(IrcMessage* message);

private:
    struct Private {
        QMultiHash<int, QTextDocument*> documents;
        QHash<IrcConnection*, CommandVerifier*> verifiers;
    } d;
};

#endif // VERIFIERPLUGIN_H
