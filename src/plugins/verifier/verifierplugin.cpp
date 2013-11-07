/*
* Copyright (C) 2008-2013 The Communi Project
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

void VerifierPlugin::initialize(IrcConnection* connection)
{
    CommandVerifier* verifier = new CommandVerifier(connection);
    connect(verifier, SIGNAL(verified(int)), this, SLOT(onCommandVerified(int)));
    d.verifiers.insert(connection, verifier);
}

void VerifierPlugin::initialize(TextDocument* document)
{
    connect(document, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
    d.highlighters.insert(document, new SyntaxHighlighter(document));
}

void VerifierPlugin::onCommandVerified(int id)
{
    TextDocument* doc = d.documents.take(id);
    SyntaxHighlighter* highlighter = d.highlighters.value(doc);
    if (doc && highlighter) {
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

void VerifierPlugin::onMessageReceived(IrcMessage* message)
{
    if (message->flags() & IrcMessage::Own) {
        TextDocument* doc = qobject_cast<TextDocument*>(sender());
        CommandVerifier* verifier = d.verifiers.value(message->connection());
        SyntaxHighlighter* highlighter = d.highlighters.value(doc);
        if (doc && verifier && highlighter) {
            QTextBlock block = doc->lastBlock();
            int id = verifier->currentId();
            block.setUserState(id);
            d.documents.insert(id, doc);
            highlighter->rehighlightBlock(block);
        }
    }
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(VerifierPlugin)
#endif
