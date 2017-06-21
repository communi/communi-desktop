/*
  Copyright (C) 2008-2016 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "browserfinder.h"
#include "textbrowser.h"
#include "textdocument.h"
#include <QWidgetAction>
#include <QActionGroup>
#include <QTextBlock>
#include <QDebug>
#include <QMenu>

BrowserFinder::BrowserFinder(TextBrowser* browser) : AbstractFinder(browser)
{
    d.textBrowser = browser;
    connect(browser, SIGNAL(documentChanged(TextDocument*)), this, SLOT(deleteLater()));
    connect(this, SIGNAL(returnPressed()), this, SLOT(findNext()));

    QMenu *menu = new QMenu(this);
    QAction *search = menu->addAction(tr("Search"));
    search->setCheckable(true);
    search->setChecked(true);
    QAction *filter = menu->addAction(tr("Filter"));
    connect(filter, SIGNAL(toggled(bool)), this, SLOT(setFilter(bool)));
    connect(this, SIGNAL(filterChanged(bool)), filter, SLOT(setChecked(bool)));
    filter->setCheckable(true);
    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    group->addAction(search);
    group->addAction(filter);

    d.menuButton = new QToolButton(lineEdit());
    d.menuButton->setObjectName("filter");
    d.menuButton->setMenu(menu);
    d.menuButton->setPopupMode(QToolButton::InstantPopup);
    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(d.menuButton);
    lineEdit()->addAction(action, QLineEdit::LeadingPosition);
}

BrowserFinder::~BrowserFinder()
{
}

void BrowserFinder::setVisible(bool visible)
{
    AbstractFinder::setVisible(visible);
    if (!visible && d.textBrowser) {
        QTextCursor cursor = d.textBrowser->textCursor();
        if (cursor.hasSelection()) {
            cursor.clearSelection();
            d.textBrowser->setTextCursor(cursor);
        }
        d.textBrowser->setExtraSelections(QList<QTextEdit::ExtraSelection>());

        QTextDocument* doc = d.textBrowser->document();
        for (QTextBlock block = doc->begin(); block != doc->end(); block = block.next()) {
            if (!block.isVisible()) {
                block.setVisible(true);
                doc->markContentsDirty(block.position(), block.length());
            }
        }
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

void BrowserFinder::filter(const QString& text)
{
    if (!d.textBrowser)
        return;

    QTextDocument* doc = d.textBrowser->document();

    bool error = true;
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!text.isEmpty()) {
        QTextCursor prevCursor(doc);
        QTextCursor findCursor(doc);
        while (!(findCursor = doc->find(text, findCursor)).isNull()) {
            error = false;
            findCursor.block().setVisible(true);
            doc->markContentsDirty(findCursor.block().position(), findCursor.block().length());
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground(Qt::yellow);
            extra.cursor = findCursor;
            extraSelections.append(extra);
            while (prevCursor.blockNumber() < findCursor.blockNumber()) {
                QTextBlock block = prevCursor.block();
                block.setVisible(false);
                doc->markContentsDirty(block.position(), block.length());
                if (!prevCursor.movePosition(QTextCursor::NextBlock))
                    break;
            }
            prevCursor = findCursor;
            prevCursor.movePosition(QTextCursor::NextBlock);
        }
        while (!prevCursor.atEnd()) {
            QTextBlock block = prevCursor.block();
            if (block.isVisible()) {
                block.setVisible(false);
                doc->markContentsDirty(block.position(), block.length());
            }
            if (!prevCursor.movePosition(QTextCursor::NextBlock))
                break;
        }
    } else {
        error = false;
        for (QTextBlock block = doc->begin(); block != doc->end(); block = block.next()) {
            if (!block.isVisible()) {
                block.setVisible(true);
                doc->markContentsDirty(block.position(), block.length());
            }
        }
    }

    if (!isVisible())
        animateShow();
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
