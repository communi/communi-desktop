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

#include "topiclabel.h"
#include <QTextDocument>
#include <IrcTextFormat>
#include <IrcCommand>
#include <IrcChannel>

TopicLabel::TopicLabel(QWidget* parent) : EditableLabel(parent)
{
    d.channel = 0;
    setOpenExternalLinks(true);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);

    connect(this, SIGNAL(edited(QString)), this, SLOT(sendTopic(QString)));
    updateTopic();
}

IrcChannel* TopicLabel::channel() const
{
    return d.channel;
}

void TopicLabel::setChannel(IrcChannel* channel)
{
    if (d.channel != channel) {
        if (d.channel)
            disconnect(d.channel, SIGNAL(topicChanged(QString)), this, SLOT(updateTopic()));
        d.channel = channel;
        if (d.channel)
            connect(d.channel, SIGNAL(topicChanged(QString)), this, SLOT(updateTopic()));
        updateTopic();
    }
}

void TopicLabel::updateTopic()
{
    const QString topic = d.channel ? d.channel->topic() : QString();
    if (topic.isEmpty())
        setText(tr("-"));
    else
        setText(IrcTextFormat().toHtml(topic));
}

void TopicLabel::sendTopic(const QString& topic)
{
    d.channel->sendCommand(IrcCommand::createTopic(d.channel->title(), topic));
}
