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

#include "documenthighlighter.h"
#include "textdocument.h"
#include <IrcConnection>
#include <IrcMessage>

DocumentHighlighter::DocumentHighlighter(TextDocument* document) : QObject(document)
{
    d.document = document;
    connect(document, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(onMessageReceived(IrcMessage*)));
}

void DocumentHighlighter::onMessageReceived(IrcMessage* message)
{
    if (message->type() == IrcMessage::Private || message->type() == IrcMessage::Notice) {
        if (!(message->flags() & IrcMessage::Own)) {
            TextDocument* document = qobject_cast<TextDocument*>(sender());
            if (document) {
                const bool contains = message->property("content").toString().contains(message->connection()->nickName(), Qt::CaseInsensitive);
                if (contains)
                    document->addHighlight(document->totalCount() - 1);
            }
        }
    }
}
