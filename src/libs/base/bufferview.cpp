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

#include "bufferview.h"
#include "textdocument.h"
#include "textbrowser.h"
#include "textinput.h"
#include "listview.h"
#include "titlebar.h"
#include <IrcBufferModel>
#include <QApplication>
#include <QVBoxLayout>
#include <IrcChannel>
#include <IrcBuffer>
#include <QShortcut>

BufferView::BufferView(QWidget* parent) : QWidget(parent)
{
    d.buffer = 0;

    d.titleBar = new TitleBar(this);
    d.listView = new ListView(this);
    d.textInput = new TextInput(this);
    d.textBrowser = new TextBrowser(this);
    d.textBrowser->setBuddy(d.textInput);

    d.textBrowser->setFocusPolicy(Qt::ClickFocus);
    d.textBrowser->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);

    d.splitter = new QSplitter(this);
    d.splitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    QShortcut* zoomIn = new QShortcut(QKeySequence::ZoomIn, this);
    zoomIn->setContext(Qt::WidgetWithChildrenShortcut);
    connect(zoomIn, SIGNAL(activated()), d.textBrowser, SLOT(zoomIn()));

    QShortcut* zoomOut = new QShortcut(QKeySequence::ZoomOut, this);
    zoomOut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(zoomOut, SIGNAL(activated()), d.textBrowser, SLOT(zoomOut()));

    QShortcut* resetZoom = new QShortcut(QKeySequence("Ctrl+0"), this);
    resetZoom->setContext(Qt::WidgetWithChildrenShortcut);
    connect(resetZoom, SIGNAL(activated()), d.textBrowser, SLOT(resetZoom()));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(d.splitter);
    layout->addWidget(d.textInput);
    layout->setStretchFactor(d.splitter, 1);

    connect(d.titleBar, SIGNAL(offsetChanged(int)), d.textBrowser, SLOT(moveShadow(int)));
    d.titleBar->raise();

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

TitleBar* BufferView::titleBar() const
{
    return d.titleBar;
}

TextBrowser* BufferView::textBrowser() const
{
    return d.textBrowser;
}

TextDocument* BufferView::textDocument() const
{
    return d.textBrowser->document();
}

void BufferView::setBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        d.buffer = buffer;

        IrcChannel* channel = qobject_cast<IrcChannel*>(buffer);
        d.listView->setChannel(channel);
        d.listView->setVisible(channel);

        d.titleBar->setBuffer(buffer);
        d.textInput->setBuffer(buffer);
        if (buffer) {
            TextDocument* doc = 0;
            QList<TextDocument*> documents = d.buffer->findChildren<TextDocument*>();
            // there might be multiple clones, but at least one instance
            // must always remain there to avoid losing history...
            Q_ASSERT(!documents.isEmpty());
            foreach (TextDocument* d, documents) {
                if (!d->isVisible())
                    doc = d;
            }
            if (!doc) {
                doc = documents.first()->clone();
                emit cloned(doc);
            }
            d.textBrowser->setDocument(doc);
        } else {
            d.textBrowser->setDocument(0);
        }

        emit bufferChanged(buffer);
    }
}

void BufferView::closeBuffer()
{
    if (d.buffer)
        emit bufferClosed(d.buffer);
}

void BufferView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    int tbh = d.titleBar->minimumSizeHint().height();
    d.titleBar->resize(width(), tbh);
    layout()->setContentsMargins(0, tbh + d.titleBar->baseOffset(), 0, 0);
}

void BufferView::query(const QString& user)
{
    IrcBufferModel* model = d.buffer ? d.buffer->model() : 0;
    if (model)
        setBuffer(model->add(user));
}
