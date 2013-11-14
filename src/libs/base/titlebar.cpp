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

#include "titlebar.h"
#include "messageformatter.h"
#include <IrcTextFormat>
#include <QStylePainter>
#include <QStyleOption>
#include <QHBoxLayout>
#include <IrcCommand>
#include <IrcChannel>

TitleBar::TitleBar(QWidget* parent) : QWidget(parent)
{
    d.buffer = 0;
    d.formatter = new MessageFormatter(this);

    d.titleLabel = new QLabel(this);
    d.titleLabel->setObjectName("title");
    d.titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d.titleLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored);

    d.topicLabel = new QLabel(this);
    d.topicLabel->setWordWrap(true);
    d.topicLabel->setObjectName("topic");
    d.topicLabel->setOpenExternalLinks(true);
    d.topicLabel->setTextFormat(Qt::RichText);
    d.topicLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    d.topicLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addSpacing(4);
    layout->addWidget(d.titleLabel);
    layout->addSpacing(4);
    layout->addWidget(d.topicLabel);
    layout->setMargin(2);

    refresh();
}

IrcBuffer* TitleBar::buffer() const
{
    return d.buffer;
}

void TitleBar::setBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        if (d.buffer) {
            IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
            if (channel) {
                disconnect(channel, SIGNAL(destroyed(IrcChannel*)), this, SLOT(cleanup()));
                disconnect(channel, SIGNAL(topicChanged(QString)), this, SLOT(refresh()));
                disconnect(channel, SIGNAL(modeChanged(QString)), this, SLOT(refresh()));
            } else {
                disconnect(d.buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(cleanup()));
            }
            disconnect(d.buffer, SIGNAL(titleChanged(QString)), this, SLOT(refresh()));
        }
        d.buffer = buffer;
        if (d.buffer) {
            IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
            if (channel) {
                connect(channel, SIGNAL(destroyed(IrcChannel*)), this, SLOT(cleanup()));
                connect(channel, SIGNAL(topicChanged(QString)), this, SLOT(refresh()));
                connect(channel, SIGNAL(modeChanged(QString)), this, SLOT(refresh()));
            } else {
                connect(d.buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(cleanup()));
            }
            connect(d.buffer, SIGNAL(titleChanged(QString)), this, SLOT(refresh()));
        }
        refresh();
    }
}

void TitleBar::sendTopic(const QString& topic)
{
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    if (channel)
        channel->sendCommand(IrcCommand::createTopic(channel->title(), topic));
}

void TitleBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QStylePainter painter(this);
    QStyleOption option;
    option.init(this);
    painter.drawPrimitive(QStyle::PE_Widget, option);
}

void TitleBar::cleanup()
{
    d.buffer = 0;
    refresh();
}

void TitleBar::refresh()
{
    d.titleLabel->setText(d.buffer ? d.buffer->title() : QString());
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    if (channel)
        d.topicLabel->setText(d.formatter->formatHtml(channel->topic().isEmpty() ? tr("-") : channel->topic()));
    else
        d.topicLabel->clear();
}
