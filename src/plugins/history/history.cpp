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

#include "history.h"
#include "textinput.h"
#include <QKeyEvent>
#include <QtDebug>

History::History(TextInput* input) : QObject(input)
{
    d.index = 0;
    d.buffer = 0;
    d.input = input;
    d.input->installEventFilter(this);
    connect(d.input, SIGNAL(bufferChanged(IrcBuffer*)), this, SLOT(changeBuffer(IrcBuffer*)));
}

bool History::eventFilter(QObject *object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::KeyPress) {
        switch (static_cast<QKeyEvent*>(event)->key()) {
            case Qt::Key_Return:
            case Qt::Key_Enter:
                if (!d.input->text().isEmpty()) {
                    d.text.clear();
                    d.history.append(d.input->text());
                    d.index = d.history.count();
                }
                event->accept();
                break;
            case Qt::Key_Up:
                if (!d.input->text().isEmpty() && !d.history.contains(d.input->text()))
                    d.text = d.input->text();
                goBackward();
                return true;
            case Qt::Key_Down:
                goForward();
                return true;
            default:
                break;
        }
    }
    return false;
}

void History::goBackward()
{
    if (d.index > 0)
        d.input->setText(d.history.value(--d.index));
}

void History::goForward()
{
    if (d.index < d.history.count())
        d.input->setText(d.history.value(++d.index));
    if (d.input->text().isEmpty())
        d.input->setText(d.text);
}

void History::changeBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        if (d.buffer)
            d.histories.insert(d.buffer, d.history);
        d.history = d.histories.value(buffer);
        d.buffer = buffer;
    }
}
