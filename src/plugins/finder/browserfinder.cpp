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

#include "browserfinder.h"
#include "textbrowser.h"
#include "textdocument.h"

BrowserFinder::BrowserFinder(TextBrowser* browser) : Finder(browser)
{
    d.textBrowser = browser;
    connect(browser, SIGNAL(documentChanged(TextDocument*)), this, SLOT(deleteLater()));
    connect(this, SIGNAL(returnPressed()), this, SLOT(findNext()));
}

BrowserFinder::~BrowserFinder()
{
}

void BrowserFinder::setVisible(bool visible)
{
    Finder::setVisible(visible);
    if (!visible && d.textBrowser) {
        QTextCursor cursor = d.textBrowser->textCursor();
        if (cursor.hasSelection()) {
            cursor.clearSelection();
            d.textBrowser->setTextCursor(cursor);
        }
        d.textBrowser->setExtraSelections(QList<QTextEdit::ExtraSelection>());
        d.textBrowser->buddy()->setFocus();
    }
}

void BrowserFinder::find(const QString& text, bool forward, bool backward, bool typed)
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
        animateShow();
    d.textBrowser->setTextCursor(newCursor);
    d.textBrowser->setExtraSelections(extraSelections);
    setError(error);
}

void BrowserFinder::relocate()
{
    QRect r = rect();
    QRect br = parentWidget()->rect();
    r.setWidth(br.width() / 3);
    r.moveBottomRight(br.bottomRight());
    r.translate(1, -offset());
    setGeometry(r);
    raise();
}
