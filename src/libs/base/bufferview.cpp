/*
  Copyright (C) 2008-2016 The Communi Project

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

    QShortcut* pageDown = new QShortcut(QKeySequence::MoveToNextPage, this);
    pageDown->setContext(Qt::WidgetWithChildrenShortcut);
    connect(pageDown, SIGNAL(activated()), d.textBrowser, SLOT(scrollToNextPage()));

    QShortcut* pageUp = new QShortcut(QKeySequence::MoveToPreviousPage, this);
    pageUp->setContext(Qt::WidgetWithChildrenShortcut);
    connect(pageUp, SIGNAL(activated()), d.textBrowser, SLOT(scrollToPreviousPage()));

    QShortcut* clearBuffer = new QShortcut(QKeySequence(tr("CTRL+K")), this);
    clearBuffer->setContext(Qt::WidgetWithChildrenShortcut);
    connect(clearBuffer, SIGNAL(activated()), d.textBrowser, SLOT(clear()));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(d.splitter);
    layout->addWidget(d.textInput);
    layout->setStretchFactor(d.splitter, 1);

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
