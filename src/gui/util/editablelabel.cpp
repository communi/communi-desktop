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

#include "editablelabel.h"
#include <QTextDocument>
#include <QTextBlock>
#include <QKeyEvent>

EditableLabel::EditableLabel(QWidget* parent) : QLabel(parent)
{
    d.editor = 0;
}

void EditableLabel::edit()
{
    d.editor = new QTextEdit(this);
    d.editor->setPlainText(findChild<QTextDocument*>()->toPlainText());
    d.editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d.editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d.editor->setAcceptRichText(false);
    d.editor->setTabChangesFocus(true);

    // keep the text visually in place...
    QTextCursor cursor(d.editor->document());
    QTextBlockFormat format = cursor.blockFormat();
    format.setIndent(1);
    cursor.setBlockFormat(format);
    d.editor->document()->setDocumentMargin(1);
    d.editor->document()->setIndentWidth(3);

    d.editor->installEventFilter(this);
    d.editor->resize(size());
    d.editor->setFocus();
    d.editor->show();
}

bool EditableLabel::eventFilter(QObject* object, QEvent* event)
{
    if (object == d.editor) {
        switch (event->type()) {
        case QEvent::FocusOut:
            d.editor->deleteLater();
            break;
        case QEvent::KeyPress: {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
                emit edited(d.editor->toPlainText());
                d.editor->deleteLater();
            }
        }
        default:
            break;
        }
    }
    return QLabel::eventFilter(object, event);
}

void EditableLabel::mouseDoubleClickEvent(QMouseEvent* event)
{
    QLabel::mouseDoubleClickEvent(event);
    edit();
    d.editor->setTextCursor(d.editor->cursorForPosition(event->pos()));
}
