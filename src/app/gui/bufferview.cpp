/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "bufferview.h"
#include "listviewex.h"
#include "topiclabel.h"
#include "textinputex.h"
#include "textdocument.h"
#include "browser.h"
#include <QVBoxLayout>
#include <IrcChannel>
#include <IrcBuffer>

BufferView::BufferView(QWidget* parent) : QWidget(parent)
{
    d.buffer = 0;
    d.browser = new Browser(this);
    d.textInput = new TextInputEx(this);
    d.topicLabel = new TopicLabel(this);
    d.listView = new ListViewEx(this);
    d.splitter = new QSplitter(this);
    d.splitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    connect(d.browser, SIGNAL(split(Qt::Orientation)), this, SIGNAL(split(Qt::Orientation)));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(d.topicLabel);
    layout->addWidget(d.splitter);
    layout->addWidget(d.textInput);

    d.splitter->setHandleWidth(1);
    d.splitter->addWidget(d.browser);
    d.splitter->addWidget(d.listView);
    d.splitter->setStretchFactor(0, 1);
}

BufferView::~BufferView()
{
}

IrcBuffer* BufferView::buffer() const
{
    return d.buffer;
}

void BufferView::setBuffer(IrcBuffer* buffer)
{
    d.buffer = buffer;
    d.browser->setDocument(buffer->property("document").value<TextDocument*>());

    IrcChannel* channel = qobject_cast<IrcChannel*>(buffer);
    d.topicLabel->setChannel(channel);
    d.topicLabel->setVisible(channel);
    d.listView->setChannel(channel);
    d.listView->setVisible(channel);

    d.textInput->setBuffer(buffer);
    if (buffer)
        d.textInput->setFocus();
}
