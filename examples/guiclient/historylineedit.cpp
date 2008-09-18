/*
* Copyright (C) 2008 J-P Nurmi jpnurmi@gmail.com
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* $Id$
*/

#include "historylineedit.h"
#include <QKeyEvent>

HistoryLineEdit::HistoryLineEdit(QWidget* parent)
	: QLineEdit(parent), index(0)
{
}

void HistoryLineEdit::goBackward()
{
    if (index > 0)
        setText(history.value(--index));
}

void HistoryLineEdit::goForward()
{
    if (index < history.count())
        setText(history.value(++index));
}

void HistoryLineEdit::clearHistory()
{
    index = 0;
    history.clear();
}

void HistoryLineEdit::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (!text().isEmpty())
            {
                history.append(text());
                index = history.count();
            }
            break;
        case Qt::Key_Up:
            goBackward();
            break;
        case Qt::Key_Down:
            goForward();
            break;
        default:
            break;
    }
    QLineEdit::keyPressEvent(event);
}
