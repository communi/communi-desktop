/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "bufferview.h"
#include "textdocument.h"
#include "textbrowser.h"
#include "topiclabel.h"
#include "textinput.h"
#include "listview.h"
#include <IrcBufferModel>
#include <QApplication>
#include <QVBoxLayout>
#include <IrcChannel>
#include <IrcBuffer>

BufferView::BufferView(QWidget* parent) : QWidget(parent)
{
    d.listView = new ListView(this);
    d.textInput = new TextInput(this);
    d.topicLabel = new TopicLabel(this);
    d.textBrowser = new TextBrowser(this);
    d.textBrowser->setBuddy(d.textInput);

    d.splitter = new QSplitter(this);
    d.splitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(d.topicLabel);
    layout->addWidget(d.splitter);
    layout->addWidget(d.textInput);

    d.splitter->addWidget(d.textBrowser);
    d.splitter->addWidget(d.listView);
    d.splitter->setStretchFactor(0, 1);

    connect(d.listView, SIGNAL(queried(QString)), this, SLOT(query(QString)));
    connect(d.textBrowser, SIGNAL(queried(QString)), this, SLOT(query(QString)));
}

BufferView::~BufferView()
{
    emit destroyed(this);
}

IrcBuffer* BufferView::buffer() const
{
    return d.buffer;
}

ListView* BufferView::listView() const
{
    return d.listView;
}

TextInput* BufferView::textInput() const
{
    return d.textInput;
}

TopicLabel* BufferView::topicLabel() const
{
    return d.topicLabel;
}

TextBrowser* BufferView::textBrowser() const
{
    return d.textBrowser;
}

void BufferView::setBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        d.buffer = buffer;

        IrcChannel* channel = qobject_cast<IrcChannel*>(buffer);
        d.topicLabel->setChannel(channel);
        d.topicLabel->setVisible(channel);
        d.listView->setChannel(channel);
        d.listView->setVisible(channel);

        d.textInput->setBuffer(buffer);
        if (buffer) {
            QWidget* focus = qApp->focusWidget();
            if (!focus || !focus->inherits("QLineEdit") || (focus->inherits("TextInput") && !isAncestorOf(focus)))
                d.textInput->setFocus();

            TextDocument* doc = 0;
            QList<TextDocument*> documents = d.buffer->findChildren<TextDocument*>();
            // there might be multiple clones, but at least one instance
            // must always remain there to avoid losing history...
            Q_ASSERT(!documents.isEmpty());
            foreach (TextDocument* d, documents) {
                if (!d->isVisible())
                    doc = d;
            }
            if (!doc)
                doc = documents.first()->clone();
            d.textBrowser->setDocument(doc);
        }

        emit bufferChanged(buffer);
    }
}

void BufferView::query(const QString& user)
{
    IrcBufferModel* model = d.buffer ? d.buffer->model() : 0;
    if (model)
        setBuffer(model->add(user));
}
