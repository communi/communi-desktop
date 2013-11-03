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

#include "editor.h"
#include "topiclabel.h"
#include <QTextDocument>
#include <QTextBlock>
#include <QKeyEvent>

Editor::Editor(TopicLabel* label) : QTextEdit(label)
{
    d.label = label;
    label->installEventFilter(this);

    setVisible(false);
    setAcceptRichText(false);
    setTabChangesFocus(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // keep the text visually in place...
    document()->setIndentWidth(3);
    document()->setDocumentMargin(1);
    QTextCursor cursor(document());
    QTextBlockFormat format = cursor.blockFormat();
    format.setIndent(1);
    cursor.setBlockFormat(format);
}

void Editor::edit()
{
    setPlainText(d.label->findChild<QTextDocument*>()->toPlainText());
    resize(d.label->size()); // TODO: layout?
    setFocus();
    show();
}

bool Editor::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    switch (event->type()) {
    case QEvent::MouseButtonDblClick:
        edit();
        setTextCursor(cursorForPosition(static_cast<QMouseEvent*>(event)->pos()));
        break;
    default:
        break;
    }
    return QTextEdit::eventFilter(object, event);
}

bool Editor::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::FocusOut:
        hide();
        break;
    case QEvent::KeyPress: {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            d.label->sendTopic(toPlainText());
            hide();
        } else if (keyEvent->key() == Qt::Key_Escape) {
            hide();
        }
    }
    default:
        break;
    }
    return QTextEdit::event(event);
}
