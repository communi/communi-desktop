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

#ifndef TOPICLABEL_H
#define TOPICLABEL_H

#include <QLabel>

class IrcChannel;
class MessageFormatter;

class TopicLabel : public QLabel
{
    Q_OBJECT

public:
    explicit TopicLabel(QWidget* parent = 0);

    IrcChannel* channel() const;

public slots:
    void setChannel(IrcChannel* channel);
    void sendTopic(const QString& topic);

signals:
    void channelChanged(IrcChannel* channel);

private slots:
    void updateTopic();

private:
    struct Private {
        IrcChannel* channel;
        MessageFormatter* formatter;
    } d;
};

#endif // TOPICLABEL_H
