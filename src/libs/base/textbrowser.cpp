/*
  Copyright (C) 2008-2015 The Communi Project

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

#include "textbrowser.h"
#include "textdocument.h"
#include <QAbstractTextDocumentLayout>
#include <QDesktopServices>
#include <QStylePainter>
#include <QStyleOption>
#include <QApplication>
#include <QTextBlock>
#include <IrcCommand>
#include <QScrollBar>
#include <IrcBuffer>
#include <QKeyEvent>
#include <QPainter>
#include <QToolTip>
#include <QAction>
#include <QMenu>

class TextShadow : public QFrame { Q_OBJECT };

TextBrowser::TextBrowser(QWidget* parent) : QTextBrowser(parent)
{
    d.bud = 0;
    d.events = true;
    d.shadow = new TextShadow;
    d.shadow->setParent(this);

    setOpenLinks(false);
    setTabChangesFocus(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, SIGNAL(anchorClicked(QUrl)), this, SLOT(onAnchorClicked(QUrl)));
}

TextBrowser::~TextBrowser()
{
    TextDocument* doc = document();
    if (doc) {
        if (doc->isClone())
            delete doc;
        else
            doc->setVisible(false);
    }
}

IrcBuffer* TextBrowser::buffer() const
{
    TextDocument* doc = document();
    if (doc)
        return doc->buffer();
    return 0;
}

TextDocument* TextBrowser::document() const
{
    return qobject_cast<TextDocument*>(QTextBrowser::document());
}

void TextBrowser::setDocument(TextDocument* document)
{
    TextDocument* doc = qobject_cast<TextDocument*>(QTextBrowser::document());
    if (doc != document) {
        if (doc) {
            doc->setVisible(false);
            disconnect(doc->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)), this, SLOT(keepAtBottom()));
            disconnect(doc, SIGNAL(lineRemoved(int)), this, SLOT(keepPosition(int)));
        }
        if (document) {
            document->setVisible(true);
            document->setDefaultFont(font());
            connect(document->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)), this, SLOT(keepAtBottom()));
            connect(document, SIGNAL(lineRemoved(int)), this, SLOT(keepPosition(int)));
        }
        connect(this, SIGNAL(textChanged()), this, SLOT(moveCursorToBottom()));
        QTextBrowser::setDocument(document);
        disconnect(this, SIGNAL(textChanged()), this, SLOT(moveCursorToBottom()));
        scrollToBottom();
        emit documentChanged(document);
    }
}

QWidget* TextBrowser::buddy() const
{
    return d.bud;
}

void TextBrowser::setBuddy(QWidget* buddy)
{
    d.bud = buddy;
}

void TextBrowser::mousePressEvent(QMouseEvent* event)
{
    const QUrl url(anchorAt(event->pos()));
    if (url.scheme() == "expand") {
        QString text;
        if (TextDocument* doc = document()) {
            const QPoint offset(horizontalScrollBar()->value(), verticalScrollBar()->value());
            text = doc->tooltip(event->pos() + offset);
        }
        if (!text.isEmpty())
            QToolTip::showText(event->globalPos(), text, viewport());
    } else if (url.scheme() == "nick") {
        QMenu* menu = createContextMenu(event->pos());
        menu->exec(event->globalPos());
        menu->deleteLater();
    }
    QTextBrowser::mousePressEvent(event);
}

void TextBrowser::mouseMoveEvent(QMouseEvent* event)
{
    QToolTip::hideText();
    QTextBrowser::mouseMoveEvent(event);
}

void TextBrowser::keyPressEvent(QKeyEvent* event)
{
    // for example:
    // - Ctrl+C goes to the browser
    // - Ctrl+V goes to the buddy
    // - Shift+7 ("/") goes to the buddy
    if (d.bud) {
        switch (event->key()) {
            case Qt::Key_Shift:
            case Qt::Key_Control:
            case Qt::Key_Meta:
            case Qt::Key_Alt:
            case Qt::Key_AltGr:
                break;
            default:
                if (!event->matches(QKeySequence::Copy) && !event->matches(QKeySequence::SelectAll)) {
                    QCoreApplication::sendEvent(d.bud, event);
                    QWidget* focus = qApp->focusWidget();
                    if (!focus || !focus->inherits("QLineEdit") || (focus->inherits("TextInput") && !isAncestorOf(focus)))
                        d.bud->setFocus();
                    return;
                }
                break;
        }
    }
    QTextBrowser::keyPressEvent(event);
}

void TextBrowser::resizeEvent(QResizeEvent* event)
{
    QTextBrowser::resizeEvent(event);

    d.shadow->resize(width(), d.shadow->height());

    // http://www.qtsoftware.com/developer/task-tracker/index_html?method=entry&id=240940
    QMetaObject::invokeMethod(this, "scrollToBottom", Qt::QueuedConnection);
}

bool TextBrowser::isAtTop() const
{
    return verticalScrollBar()->value() <= verticalScrollBar()->minimum();
}

bool TextBrowser::isAtBottom() const
{
    return verticalScrollBar()->value() >= verticalScrollBar()->maximum();
}

bool TextBrowser::isZoomed() const
{
    QFont f = font();
    if (f.pointSize() != -1)
        return f.pointSize() != QFont().pointSize();
    return f.pixelSize() != QFont().pixelSize();
}

QMenu* TextBrowser::createContextMenu(const QPoint& pos)
{
    // QTextEdit::createStandardContextMenu() expects document coordinates
    QPoint docPos = pos;
    QScrollBar* hbar = horizontalScrollBar();
    docPos.rx() += isRightToLeft() ? hbar->maximum() - hbar->value() : hbar->value();
    docPos.ry() += verticalScrollBar()->value();

    QMenu* menu = createStandardContextMenu(docPos);

    const QString anchor = anchorAt(pos);
    if (anchor.startsWith("nick:")) {
        // disable "Copy Link Location" for nicks
        QAction* action = menu->actions().value(1);
        if (action)
            action->setDisabled(true);

        // inject query & whois
        QAction* separator = menu->insertSeparator(menu->actions().value(0));

        QAction* queryAction = new QAction(tr("Query"), menu);
        menu->insertAction(separator, queryAction);
        connect(queryAction, SIGNAL(triggered()), this, SLOT(onQueryTriggered()));

        QAction* whoisAction = new QAction(tr("Whois"), menu);
        menu->insertAction(queryAction, whoisAction);
        connect(whoisAction, SIGNAL(triggered()), this, SLOT(onWhoisTriggered()));

        QString nick = QUrl(anchor).toString(QUrl::RemoveScheme | QUrl::RemoveFragment | QUrl::FullyDecoded);

        QAction* nickAction = new QAction(nick, menu);
        menu->insertAction(whoisAction, nickAction);
        nickAction->setEnabled(false);
        menu->insertSeparator(whoisAction);

        nickAction->setText(nick);
        queryAction->setData(nick);
        whoisAction->setData(nick);
    }
    return menu;
}

void TextBrowser::clear()
{
    QTextBrowser::clear();
    TextDocument* doc = document();
    if (doc)
        doc->reset();
}

void TextBrowser::resetZoom()
{
    QFont f = font();
    f.setPointSize(QFont().pointSize());
    setFont(f);
}

void TextBrowser::scrollToTop()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderToMinimum);
}

void TextBrowser::scrollToBottom()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderToMaximum);
}

static void scrollPage(QScrollBar* bar, qreal f)
{
    int add = f * bar->pageStep();
    int pos = bar->value() + add;
    if (add > 0 && pos < bar->value())
        pos = bar->maximum();
    else if (add < 0 && pos > bar->value())
        pos = bar->minimum();
    bar->setSliderPosition(pos);
}

void TextBrowser::scrollToNextPage()
{
    scrollPage(verticalScrollBar(), 0.667);
}

void TextBrowser::scrollToPreviousPage()
{
    scrollPage(verticalScrollBar(), -0.667);
}

void TextBrowser::paintEvent(QPaintEvent* event)
{
    // TODO: make sure the shadow is always on top (of transient scrollbars)
    d.shadow->raise();

    const int hoffset = horizontalScrollBar()->value();
    const int voffset = verticalScrollBar()->value();
    const QRect bounds = rect().translated(hoffset, voffset);

    TextDocument* doc = document();
    if (doc) {
        QPainter painter(viewport());
        painter.translate(-hoffset, -voffset);
        doc->drawBackground(&painter, bounds);
    }

    QTextBrowser::paintEvent(event);

    if (doc) {
        QPainter painter(viewport());
        painter.translate(-hoffset, -voffset);
        doc->drawForeground(&painter, bounds);
    }
}

void TextBrowser::wheelEvent(QWheelEvent* event)
{
#ifdef Q_OS_MAC
    // disable cmd+wheel zooming on mac
    QAbstractScrollArea::wheelEvent(event);
#else
    QTextBrowser::wheelEvent(event);
#endif // Q_OS_MAC
}

void TextBrowser::keepAtBottom()
{
    if (isAtBottom())
        QMetaObject::invokeMethod(this, "scrollToBottom", Qt::QueuedConnection);
}

void TextBrowser::keepPosition(int delta)
{
    if (!isAtBottom())
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta);
}

void TextBrowser::moveCursorToBottom()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

void TextBrowser::moveShadow(int offset)
{
    d.shadow->move(0, offset);
}

void TextBrowser::onAnchorClicked(const QUrl& url)
{
    if (url.scheme() != "expand" && url.scheme() != "nick")
        QDesktopServices::openUrl(url);
    clearFocus();
    d.bud->setFocus();
}

void TextBrowser::onWhoisTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        IrcCommand* command = IrcCommand::createWhois(action->data().toString());
        buffer()->sendCommand(command);
    }
}

void TextBrowser::onQueryTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
        emit queried(action->data().toString());
}

#include "textbrowser.moc"
