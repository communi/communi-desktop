/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#include "historylineedit.h"
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>

HistoryLineEdit::HistoryLineEdit(QWidget* parent)
    : QLineEdit(parent), index(0)
{
}

// QLineEdit doesn't like chars like '-', '/'..
// so we'll do it by hand
void HistoryLineEdit::cursorWordBackward(bool mark)
{
    // skip trailing whitespace
    while (cursorPosition() > 0 && text().at(cursorPosition() - 1).isSpace())
        cursorBackward(false, 1);

    // find previous whitespace
    int steps = cursorPosition();
    int idx = text().lastIndexOf(QRegExp("\\s"), steps - 1);

    // move cursor
    if (idx != -1)
        steps -= idx + 1;
    cursorBackward(mark, steps);
}

// QLineEdit::insert() emits textEdited()
// so here's the workaround
void HistoryLineEdit::insert(const QString& text)
{
    QString tmp = QLineEdit::text();
    int pos = cursorPosition();
    int len = selectedText().length();
    tmp.replace(pos, len, text);
    setText(tmp);
    setCursorPosition(pos + text.length() + 1);
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
    if (text().isEmpty())
        setText(input);
}

void HistoryLineEdit::clearHistory()
{
    index = 0;
    input.clear();
    history.clear();
}

void HistoryLineEdit::keyPressEvent(QKeyEvent* event)
{
    if (event == QKeySequence::Paste)
    {
        QStringList lines = qApp->clipboard()->text().split(QRegExp("\n"), QString::SkipEmptyParts);
        // TODO: confirm if too many lines
        if (!lines.isEmpty())
        {
            for (int i = 0; i < lines.count() - 1; ++i)
            {
                insert(lines.at(i));
                QKeyEvent keyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
                keyPressEvent(&keyEvent);
            }
            insert(lines.last());
        }
        return;
    }

    switch (event->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (!text().isEmpty())
            {
                input.clear();
                history.append(text());
                index = history.count();
            }
            event->accept();
            break;
        case Qt::Key_Up:
            if (!text().isEmpty() && !history.contains(text()))
                input = text();
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
    QLineEdit::keyPressEvent(event);
}

bool HistoryLineEdit::event(QEvent* event)
{
    bool ret = QLineEdit::event(event);
    if (event->type() == QEvent::ShortcutOverride)
        ret = false;
    return ret;
}
