/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "bufferview.h"
#include "textbrowser.h"
#include "textdocument.h"
#include "messageformatter.h"
#include <QDesktopServices>
#include <IrcUserModel>
#include <IrcChannel>

BufferView::BufferView(QWidget* parent) : QStackedWidget(parent)
{
    d.bud = 0;
    d.buffer = 0;
    d.userModel = new IrcUserModel(this);
    d.formatter = new MessageFormatter(this);
}

BufferView::~BufferView()
{
}

IrcBuffer* BufferView::currentBuffer() const
{
    return d.buffer;
}

QWidget* BufferView::buddy() const
{
    return d.bud;
}

void BufferView::setBuddy(QWidget* buddy)
{
    d.bud = buddy;
    foreach (TextBrowser* browser, d.browsers)
        browser->setBuddy(buddy);
}

void BufferView::addBuffer(IrcBuffer* buffer)
{
    connect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(receiveMessage(IrcMessage*)));

    TextBrowser* browser = new TextBrowser(this);
    TextDocument* doc = new TextDocument(browser);
    doc->setMaximumBlockCount(1000);
    doc->setDefaultStyleSheet(d.css);
    browser->setBuddy(d.bud);
    browser->setDocument(doc);
    browser->setOpenLinks(false);
    browser->setTabChangesFocus(true);
    connect(browser, SIGNAL(anchorClicked(QUrl)), this, SLOT(onAnchorClicked(QUrl)));

    addWidget(browser);
    d.browsers.insert(buffer, browser);

    if (buffer == d.buffer) // TODO:
        setCurrentWidget(browser);
}

void BufferView::removeBuffer(IrcBuffer* buffer)
{
    disconnect(buffer, SIGNAL(messageReceived(IrcMessage*)), this, SLOT(receiveMessage(IrcMessage*)));
    delete d.browsers.value(buffer);
}

void BufferView::setCurrentBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        if (d.buffer) {
            TextBrowser* browser = d.browsers.value(d.buffer);
            if (browser) {
                browser->document()->setNote(-1);
                browser->document()->setActive(false);
            }
        }
        d.buffer = buffer;
        if (buffer) {
            d.userModel->setChannel(qobject_cast<IrcChannel*>(buffer));
            TextBrowser* browser = d.browsers.value(buffer);
            if (browser) {
                browser->document()->setActive(true);
                setCurrentWidget(browser);
                browser->scrollToBottom();
            }
        }
    }
}

void BufferView::onAnchorClicked(const QUrl& url)
{
    if (url.scheme() == "nick")
        emit clicked(url.toString(QUrl::RemoveScheme));
    else
        QDesktopServices::openUrl(url);
    clearFocus();
}

void BufferView::receiveMessage(IrcMessage* message)
{
    IrcBuffer* buffer = qobject_cast<IrcBuffer*>(sender());
    if (!buffer)
        buffer = currentBuffer();
    TextBrowser* browser = d.browsers.value(buffer);
    if (browser) {
        TextDocument* doc = browser->document();
        const bool atBottom = browser->isAtBottom();

        d.formatter->setBuffer(buffer);
        const QString formatted = d.formatter->formatMessage(message);
        bool highlight = d.formatter->wasHighlighted();
        doc->append(formatted, highlight);

        if (buffer == d.buffer) {
            if (atBottom)
                browser->scrollToBottom();
        } else {
            // set the "unseen block" note
            int note = doc->note();
            if (note == -1) {
                note = doc->totalCount() - 1;
                if (note > 1)
                    doc->setNote(note);
            }
            // highlight non-current private queries
            if (!qobject_cast<IrcChannel*>(buffer))
                highlight = true;
        }
        if (highlight) {
            emit highlighted(message);
            emit highlighted(buffer);
        }
    }
}
