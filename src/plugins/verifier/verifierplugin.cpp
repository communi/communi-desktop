/*
* Copyright (C) 2008-2014 The Communi Project
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
    connect(verifier, SIGNAL(verified(quint64)), this, SLOT(onCommandVerified(quint64)));
    d.verifiers.insert(connection, verifier);
}

void VerifierPlugin::documentAdded(TextDocument* document)
{
    new SyntaxHighlighter(document);
    connect(document, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

void VerifierPlugin::onCommandVerified(quint64 id)
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
    if (message->flags() & IrcMessage::Own) {
        TextDocument* doc = qobject_cast<TextDocument*>(sender());
        CommandVerifier* verifier = d.verifiers.value(message->connection());
        if (doc && verifier) {
            quint64 id = verifier->identify(message);
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
