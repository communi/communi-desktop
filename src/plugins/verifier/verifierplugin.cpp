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

#include "verifierplugin.h"
#include "commandverifier.h"
#include "syntaxhighlighter.h"
#include "textdocument.h"
#include <IrcMessage>
#include <IrcBuffer>

VerifierPlugin::VerifierPlugin(QObject* parent) : QObject(parent)
{
}

void VerifierPlugin::connectionAdded(IrcConnection* connection)
{
    CommandVerifier* verifier = new CommandVerifier(connection);
    connect(verifier, SIGNAL(verified(int)), this, SLOT(onCommandVerified(int)));
    d.verifiers.insert(connection, verifier);
}

void VerifierPlugin::documentAdded(TextDocument* document)
{
    new SyntaxHighlighter(document);
    connect(document, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

void VerifierPlugin::onCommandVerified(int id)
{
    foreach (QTextDocument* doc, d.documents.values(id)) {
        SyntaxHighlighter* highlighter = doc->findChild<SyntaxHighlighter*>();
        if (highlighter) {
            QTextBlock block = doc->lastBlock();
            while (block.isValid()) {
                if (block.userState() == id) {
                    block.setUserState(-1);
                    highlighter->rehighlightBlock(block);
                    break;
                }
                block = block.previous();
            }
        }
    }
    d.documents.remove(id);
}

void VerifierPlugin::onMessageReceived(IrcMessage* message)
{
    if (message->isOwn()) {
        QTextDocument* doc = qobject_cast<QTextDocument*>(sender());
        CommandVerifier* verifier = d.verifiers.value(message->connection());
        if (doc && verifier) {
            int id = verifier->identify(message);
            if (id > 1) {
                SyntaxHighlighter* highlighter = doc->findChild<SyntaxHighlighter*>();
                if (highlighter) {
                    QTextBlock block = doc->lastBlock();
                    block.setUserState(id);
                    d.documents.insertMulti(id, doc);
                    highlighter->rehighlightBlock(block);
                }
            }
        }
    }
}
