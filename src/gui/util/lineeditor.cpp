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

#include "lineeditor.h"
#include <QKeyEvent>

LineEditor::LineEditor(QWidget* parent) : FancyLineEdit(parent)
{
    d.index = 0;
}

QStringList LineEditor::history() const
{
    return d.history;
}

void LineEditor::setHistory(const QStringList& history)
{
    d.index = history.count();
    d.history = history;
}

void LineEditor::clearHistory()
{
    setHistory(QStringList());
}

void LineEditor::goBackward()
{
    if (d.index > 0)
        setText(d.history.value(--d.index));
}

void LineEditor::goForward()
{
    if (d.index < d.history.count())
        setText(d.history.value(++d.index));
    if (text().isEmpty())
        setText(d.input);
}

void LineEditor::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (!text().isEmpty()) {
                d.input.clear();
                d.history.append(text());
                d.index = d.history.count();
            }
            event->accept();
            break;
        case Qt::Key_Up:
            if (!text().isEmpty() && !d.history.contains(text()))
                d.input = text();
            goBackward();
            event->accept();
            return;
        case Qt::Key_Down:
            goForward();
            event->accept();
            return;
        default:
            break;
    }
    FancyLineEdit::keyPressEvent(event);
}
