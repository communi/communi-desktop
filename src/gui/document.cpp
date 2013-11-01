/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "document.h"
#include "messageformatter.h"
#include <IrcBuffer>
#include <QtDebug>

typedef QHash<IrcBuffer*, Document*> BufferDocuments;
Q_GLOBAL_STATIC(BufferDocuments, bufferDocuments)

Document::Document(IrcBuffer* buffer) : TextDocument(buffer)
{
    setMaximumBlockCount(1000);

    d.buffer = buffer;
    if (buffer) {
        d.formatter = new MessageFormatter(this);
        d.formatter->setBuffer(buffer);

        connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(receiveMessage(IrcMessage*)));
    }
}

Document::~Document()
{
}

IrcBuffer* Document::buffer() const
{
    return d.buffer;
}

Document* Document::instance(IrcBuffer* buffer)
{
    static Document doc(0);
    if (buffer) {
        if (buffer && !bufferDocuments()->contains(buffer))
            bufferDocuments()->insert(buffer, new Document(buffer));
        return bufferDocuments()->value(buffer);
    }
    return &doc;
}

void Document::addBuffer(IrcBuffer* buffer)
{
    bufferDocuments()->insert(buffer, new Document(buffer));
}

void Document::removeBuffer(IrcBuffer* buffer)
{
    delete bufferDocuments()->take(buffer);
}

void Document::receiveMessage(IrcMessage* message)
{
    const QString formatted = d.formatter->formatMessage(message);
    const bool highlight = d.formatter->wasHighlighted();
    append(formatted, highlight);
}
