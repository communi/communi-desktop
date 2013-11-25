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
#include <QStyleOptionHeader>
#include <QStylePainter>
#include <IrcTextFormat>
#include <IrcCommand>
#include <IrcChannel>
#include <QStyle>

TitleBar::TitleBar(QWidget* parent) : QLabel(parent)
{
    d.buffer = 0;
    d.formatter = new MessageFormatter(this);

    setWordWrap(true);
    setOpenExternalLinks(true);
    setTextFormat(Qt::RichText);

#ifdef Q_OS_MAC
    QFont font;
    font.setPointSize(11.0);
    setFont(font);
#endif
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

QString TitleBar::topic() const
{
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    if (channel)
        return channel->topic();
    return QString();
}

void TitleBar::setTopic(const QString& topic)
{
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    if (channel) {
        if (channel->topic() != topic)
            channel->sendCommand(IrcCommand::createTopic(channel->title(), topic));
    }
}

void TitleBar::paintEvent(QPaintEvent* event)
{
    QStyleOptionHeader option;
    option.init(this);
    option.state = (QStyle::State_Raised | QStyle::State_Horizontal);
    option.position = QStyleOptionHeader::OnlyOneSection;
    QStylePainter painter(this);
    painter.drawControl(QStyle::CE_HeaderSection, option);

    QLabel::paintEvent(event);
}

void TitleBar::resizeEvent(QResizeEvent* event)
{
    QStyleOptionHeader option;
    option.init(this);
    option.state = (QStyle::State_Raised | QStyle::State_Horizontal);
    option.position = QStyleOptionHeader::OnlyOneSection;
    QRect ser = style()->subElementRect(QStyle::SE_HeaderLabel, &option, this);
    setContentsMargins(ser.x(), ser.y(), width () - ser.x() - ser.width(), height() - ser.y() - ser.height());

    QLabel::resizeEvent(event);
}

void TitleBar::cleanup()
{
    d.buffer = 0;
    refresh();
}

void TitleBar::refresh()
{
    QString title = d.buffer ? d.buffer->title() : QString();
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    QString topic = channel ? channel->topic() : QString();
    if (!topic.isEmpty())
        title = tr("%1: %2").arg(title, d.formatter->formatHtml(topic));
    setText(title);
}
