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

#include "searcheditor.h"
#include <QTextEdit>
#include <QShortcut>

SearchEditor::SearchEditor(QWidget* parent) : HistoryLineEdit(parent)
{
    Q_ASSERT(parent);
    d.textEdit = 0;

    QShortcut* shortcut = new QShortcut(QKeySequence::Find, parent);
    connect(shortcut, SIGNAL(activated()), this, SLOT(find()));

    shortcut = new QShortcut(QKeySequence::FindNext, parent);
    connect(shortcut, SIGNAL(activated()), this, SLOT(findNext()));

    shortcut = new QShortcut(QKeySequence::FindPrevious, parent);
    connect(shortcut, SIGNAL(activated()), this, SLOT(findPrevious()));

    setButtonVisible(Left, true);
    setAutoHideButton(Left, true);
    connect(this, SIGNAL(leftButtonClicked()), this, SLOT(findPrevious()));

    setButtonVisible(Right, true);
    setAutoHideButton(Right, true);
    connect(this, SIGNAL(rightButtonClicked()), this, SLOT(findNext()));

    connect(this, SIGNAL(returnPressed()), this, SLOT(findNext()));
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(find(QString)));

    setAttribute(Qt::WA_MacShowFocusRect, false);
    setVisible(false);
}

QTextEdit* SearchEditor::textEdit() const
{
    return d.textEdit;
}

void SearchEditor::setTextEdit(QTextEdit* textEdit)
{
    d.textEdit = textEdit;
}

void SearchEditor::findNext()
{
    find(text(), true, false, false);
}

void SearchEditor::findPrevious()
{
    find(text(), false, true, false);
}

void SearchEditor::find()
{
    show();
    setFocus(Qt::ShortcutFocusReason);
    selectAll();
}

void SearchEditor::find(const QString& text, bool forward, bool backward, bool typed)
{
    if (!d.textEdit)
        return;

    QTextDocument* doc = d.textEdit->document();
    QTextCursor cursor = d.textEdit->textCursor();

    bool error = false;
    QTextDocument::FindFlags options;

    if (cursor.hasSelection())
        cursor.setPosition(typed ? cursor.selectionEnd() : forward ? cursor.position() : cursor.anchor(), QTextCursor::MoveAnchor);

    QTextCursor newCursor = cursor;
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!text.isEmpty()) {
        if (typed || backward)
            options |= QTextDocument::FindBackward;

        newCursor = doc->find(text, cursor, options);

        if (newCursor.isNull()) {
            QTextCursor ac(doc);
            ac.movePosition(options & QTextDocument::FindBackward
                            ? QTextCursor::End : QTextCursor::Start);
            newCursor = doc->find(text, ac, options);
            if (newCursor.isNull()) {
                error = true;
                newCursor = cursor;
            }
        }

        QTextCursor findCursor(doc);
        while (!(findCursor = doc->find(text, findCursor)).isNull()) {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground(Qt::yellow);
            extra.cursor = findCursor;
            extraSelections.append(extra);
        }
    }

    if (!isVisible())
        show();
    d.textEdit->setTextCursor(newCursor);
    d.textEdit->setExtraSelections(extraSelections);
    setStyleSheet(error ? "background: #dc322f" : "");
}

void SearchEditor::hideEvent(QHideEvent* event)
{
    HistoryLineEdit::hideEvent(event);
    if (d.textEdit) {
        QTextCursor cursor = d.textEdit->textCursor();
        if (cursor.hasSelection()) {
            cursor.clearSelection();
            d.textEdit->setTextCursor(cursor);
        }
        d.textEdit->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    }
}
