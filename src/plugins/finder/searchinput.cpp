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

#include "searchinput.h"
#include "textbrowser.h"
#include "textdocument.h"
#include <QShortcut>

SearchInput::SearchInput(TextBrowser* browser) : QLineEdit(browser)
{
    d.textBrowser = browser;

    QShortcut* shortcut = new QShortcut(QKeySequence::Find, browser);
    connect(shortcut, SIGNAL(activated()), this, SLOT(find()));

    shortcut = new QShortcut(QKeySequence::FindNext, browser);
    connect(shortcut, SIGNAL(activated()), this, SLOT(findNext()));

    shortcut = new QShortcut(QKeySequence::FindPrevious, browser);
    connect(shortcut, SIGNAL(activated()), this, SLOT(findPrevious()));

//    TODO:
//    setButtonVisible(Left, true);
//    setAutoHideButton(Left, true);
//    setButtonPixmap(Left, QPixmap(":/icons/buttons/prev.png"));
//    connect(this, SIGNAL(leftButtonClicked()), this, SLOT(findPrevious()));

//    setButtonVisible(Right, true);
//    setAutoHideButton(Right, true);
//    setButtonPixmap(Right, QPixmap(":/icons/buttons/next.png"));
//    connect(this, SIGNAL(rightButtonClicked()), this, SLOT(findNext()));

    connect(this, SIGNAL(returnPressed()), this, SLOT(findNext()));
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(find(QString)));

    setAttribute(Qt::WA_MacShowFocusRect, false);
    setVisible(false);
}

void SearchInput::findNext()
{
    find(text(), true, false, false);
}

void SearchInput::findPrevious()
{
    find(text(), false, true, false);
}

void SearchInput::find()
{
    show();
    setFocus(Qt::ShortcutFocusReason);
    selectAll();
}

void SearchInput::find(const QString& text, bool forward, bool backward, bool typed)
{
    if (!d.textBrowser)
        return;

    QTextDocument* doc = d.textBrowser->document();
    QTextCursor cursor = d.textBrowser->textCursor();

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
    d.textBrowser->setTextCursor(newCursor);
    d.textBrowser->setExtraSelections(extraSelections);
    setStyleSheet(error ? "background: #ff7a7a" : "");
}

void SearchInput::hideEvent(QHideEvent* event)
{
    QLineEdit::hideEvent(event);
    if (d.textBrowser) {
        QTextCursor cursor = d.textBrowser->textCursor();
        if (cursor.hasSelection()) {
            cursor.clearSelection();
            d.textBrowser->setTextCursor(cursor);
        }
        d.textBrowser->setExtraSelections(QList<QTextEdit::ExtraSelection>());
    }
}
