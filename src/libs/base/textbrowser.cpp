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

#include "textbrowser.h"
#include "textdocument.h"
#include <QDesktopServices>
#include <QApplication>
#include <QScrollBar>
#include <QKeyEvent>
#include <QPainter>

TextBrowser::TextBrowser(QWidget* parent) : QTextBrowser(parent)
{
    d.bud = 0;
    d.dirty = 0;
    d.markerColor = Qt::darkGray;
    d.highlightColor = QColor("#ffe6e6"); // TODO: stylesheet

    setOpenLinks(false);
    setTabChangesFocus(true);

    connect(this, SIGNAL(anchorClicked(QUrl)), this, SLOT(onAnchorClicked(QUrl)));
}

TextDocument* TextBrowser::document() const
{
    return qobject_cast<TextDocument*>(QTextBrowser::document());
}

void TextBrowser::setDocument(TextDocument* document)
{
    TextDocument* doc = qobject_cast<TextDocument*>(QTextBrowser::document());
    if (doc != document) {
        if (doc)
            doc->deref(this);
        if (document) {
            document->ref(this);
            if (d.dirty == 0 && !document->isEmpty()) {
                setUpdatesEnabled(false);
                d.dirty = startTimer(32);
            }
            QMetaObject::invokeMethod(this, "scrollToBottom", Qt::QueuedConnection);
        }
        QTextBrowser::setDocument(document);
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

QColor TextBrowser::markerColor() const
{
    return d.markerColor;
}

void TextBrowser::setMarkerColor(const QColor& color)
{
    if (d.markerColor != color) {
        d.markerColor = color;
        update();
    }
}

QColor TextBrowser::highlightColor() const
{
    return d.highlightColor;
}

void TextBrowser::setHighlightColor(const QColor& color)
{
    if (d.highlightColor != color) {
        d.highlightColor = color;
        update();
    }
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

    // http://www.qtsoftware.com/developer/task-tracker/index_html?method=entry&id=240940
    QMetaObject::invokeMethod(this, "scrollToBottom", Qt::QueuedConnection);
}

void TextBrowser::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == d.dirty) {
        setUpdatesEnabled(true);
        killTimer(d.dirty);
        d.dirty = 0;
    }
}

bool TextBrowser::isAtTop() const
{
    return verticalScrollBar()->value() <= verticalScrollBar()->minimum();
}

bool TextBrowser::isAtBottom() const
{
    return verticalScrollBar()->value() >= verticalScrollBar()->maximum();
}

void TextBrowser::scrollToTop()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderToMinimum);
}

void TextBrowser::scrollToBottom()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderToMaximum);
}

void TextBrowser::scrollToNextPage()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepAdd);
}

void TextBrowser::scrollToPreviousPage()
{
    verticalScrollBar()->triggerAction(QScrollBar::SliderPageStepSub);
}

void TextBrowser::paintEvent(QPaintEvent* event)
{
    const int hoffset = horizontalScrollBar()->value();
    const int voffset = verticalScrollBar()->value();
    const QRect bounds = rect().translated(hoffset, voffset);

    TextDocument* doc = document();
    if (doc) {
        QPainter painter(viewport());
        painter.translate(-hoffset, -voffset);
        painter.setPen(Qt::NoPen);
        painter.setBrush(d.highlightColor);
        doc->drawHighlights(&painter, bounds);
    }

    QTextBrowser::paintEvent(event);

    if (doc) {
        QPainter painter(viewport());
        painter.translate(-hoffset, -voffset);
        painter.setPen(QPen(d.markerColor, 1, Qt::DashLine));
        painter.setBrush(Qt::NoBrush);
        doc->drawMarkers(&painter, bounds);
    }

    QLinearGradient gradient(0, 0, 0, 3);
    gradient.setColorAt(0.0, palette().color(QPalette::Dark));
    gradient.setColorAt(1.0, Qt::transparent);

    QPainter painter(viewport());
    painter.fillRect(0, 0, width(), 3, gradient);
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

void TextBrowser::onAnchorClicked(const QUrl& url)
{
    if (url.scheme() == "nick")
        emit queried(url.toString(QUrl::RemoveScheme));
    else
        QDesktopServices::openUrl(url);
    clearFocus();
}
