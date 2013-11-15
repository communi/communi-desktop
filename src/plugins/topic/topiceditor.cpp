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

#include "topiceditor.h"
#include "titlebar.h"
#include <QTextDocument>
#include <QTextBlock>
#include <IrcChannel>
#include <QKeyEvent>

TopicEditor::TopicEditor(TitleBar* bar) : QTextEdit(bar)
{
    d.bar = bar;
    QLabel* label = bar->findChild<QLabel*>("title");
    if (label)
        label->installEventFilter(this);

    setVisible(false);
    setAcceptRichText(false);
    setTabChangesFocus(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void TopicEditor::edit()
{
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.bar->buffer());
    if (channel) {
        setPlainText(channel->topic());

        // keep the text visually in place...
        document()->setIndentWidth(3);
        document()->setDocumentMargin(1);
        QTextCursor cursor(document());
        QTextBlockFormat format = cursor.blockFormat();
        format.setIndent(1);
        cursor.setBlockFormat(format);

        resize(parentWidget()->size()); // TODO: layout?
        setFocus();
        show();
    }
}

bool TopicEditor::eventFilter(QObject* object, QEvent* event)
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

bool TopicEditor::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::FocusOut:
        hide();
        break;
    case QEvent::KeyPress: {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            d.bar->setTopic(toPlainText());
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
