/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "textdocument.h"

class IrcBuffer;
class IrcMessage;
class MessageFormatter;

class Document : public TextDocument
{
    Q_OBJECT

public:
    Document(IrcBuffer* buffer);
    ~Document();

    IrcBuffer* buffer() const;

    static Document* instance(IrcBuffer* buffer = 0);

public slots:
    static void addBuffer(IrcBuffer* buffer);
    static void removeBuffer(IrcBuffer* buffer);

private slots:
    void receiveMessage(IrcMessage* message);

private:
    struct Private {
        IrcBuffer* buffer;
        MessageFormatter* formatter;
    } d;
};

#endif // DOCUMENT_H
