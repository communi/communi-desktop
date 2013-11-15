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

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QLabel>

class IrcBuffer;
class MessageFormatter;

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget* parent = 0);

    IrcBuffer* buffer() const;
    QString topic() const;

public slots:
    void setBuffer(IrcBuffer* buffer);
    void setTopic(const QString& topic);

signals:
    void bufferChanged(IrcBuffer* buffer);

protected:
    void paintEvent(QPaintEvent* event);

private slots:
    void cleanup();
    void refresh();

private:
    struct Private {
        QLabel* label;
        IrcBuffer* buffer;
        MessageFormatter* formatter;
    } d;
};

#endif // TITLEBAR_H
