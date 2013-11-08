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
#include <QPropertyAnimation>
#include <QStylePainter>
#include <QStyleOption>
#include <QHBoxLayout>
#include <QShortcut>
#include <QEvent>

SearchInput::SearchInput(TextBrowser* browser) : QWidget(browser)
{
    d.offset = -1;

    d.textBrowser = browser;
    browser->installEventFilter(this);
    connect(browser, SIGNAL(documentChanged(TextDocument*)), this, SLOT(reset()));

    d.lineEdit = new QLineEdit(this);
    d.lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

    d.prevButton = new QToolButton(this);
    d.prevButton->setIcon(QIcon(":/icons/prev.png"));
    connect(d.prevButton, SIGNAL(clicked()), this, SLOT(findPrevious()));

    d.nextButton = new QToolButton(this);
    d.nextButton->setIcon(QIcon(":/icons/next.png"));
    connect(d.nextButton, SIGNAL(clicked()), this, SLOT(findNext()));

    d.closeButton = new QToolButton(this);
    d.closeButton->setIcon(QIcon(":/icons/close.png"));
    connect(d.closeButton, SIGNAL(clicked()), this, SLOT(animateHide()));

    d.lineEdit->setStyleSheet(
    "QLineEdit {"
        "border: 1px solid palette(dark);"
        "border-top-left-radius: 4px;"
        "border-bottom-left-radius: 4px;"
    "}"
    "QLineEdit#error {"
        "background: #ff7a7a"
    "}");

    d.prevButton->setStyleSheet(
    "QToolButton {"
        "border: 1px solid palette(dark);"
        "border-left-color: transparent;"
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                          "stop: 0 palette(light), stop: 1 palette(button));"
    "}"
    "QToolButton:pressed {"
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                          "stop: 0 palette(button), stop: 1 palette(light));"
    "}");

    d.nextButton->setStyleSheet(
    "QToolButton {"
        "border: 1px solid palette(dark);"
        "border-left-color: transparent;"
        "border-top-right-radius: 4px;"
        "border-bottom-right-radius: 4px;"
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                          "stop: 0 palette(light), stop: 1 palette(button));"
    "}"
    "QToolButton:pressed {"
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                          "stop: 0 palette(button), stop: 1 palette(light));"
    "}");

    d.closeButton->setStyleSheet(
    "QToolButton {"
        "margin: 3px;"
        "border-radius: 6px;"
        "background-color: transparent;"
    "}"
    "QToolButton:hover {"
        "background-color: #eaaaaa;"
        "border: 1px solid palette(dark);"
    "}"
    "QToolButton:pressed {"
        "background-color: #d57070;"
        "border: 1px solid palette(dark);"
    "}");

    setObjectName("container");
    setStyleSheet(
    "QWidget#container {"
        "background-color: palette(window);"
        "border: 1px solid palette(dark);"
        "border-bottom-left-radius: 4px;"
    "}");

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(d.closeButton);
    layout->addWidget(d.lineEdit);
    layout->addWidget(d.prevButton);
    layout->addWidget(d.nextButton);
    layout->setSpacing(0);
    layout->setMargin(3);

    d.prevButton->setFixedHeight(d.lineEdit->sizeHint().height());
    d.nextButton->setFixedHeight(d.lineEdit->sizeHint().height());
    d.closeButton->setFixedHeight(d.lineEdit->sizeHint().height());
    d.closeButton->setFixedWidth(d.lineEdit->sizeHint().height());

    QShortcut* shortcut = new QShortcut(QKeySequence::Find, browser);
    connect(shortcut, SIGNAL(activated()), this, SLOT(find()));

    shortcut = new QShortcut(QKeySequence::FindNext, browser);
    connect(shortcut, SIGNAL(activated()), this, SLOT(findNext()));

    shortcut = new QShortcut(QKeySequence::FindPrevious, browser);
    connect(shortcut, SIGNAL(activated()), this, SLOT(findPrevious()));

    shortcut = new QShortcut(QKeySequence(tr("Esc")), browser);
    connect(shortcut, SIGNAL(activated()), this, SLOT(animateHide()));

    connect(d.lineEdit, SIGNAL(returnPressed()), this, SLOT(findNext()));
    connect(d.lineEdit, SIGNAL(textEdited(QString)), this, SLOT(find(QString)));

    setOffset(-1);
    setVisible(false);
}

int SearchInput::offset() const
{
    return d.offset;
}

void SearchInput::setOffset(int offset)
{
    d.offset = offset;
    move(x(), d.offset);
}

bool SearchInput::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::Resize) {
        QRect r = rect();
        QRect br = d.textBrowser->rect();
        r.setWidth(br.width() / 3);
        r.moveTopRight(br.topRight());
        r.translate(1, -1);
        setGeometry(r);
    }
    return false;
}

void SearchInput::findNext()
{
    find(d.lineEdit->text(), true, false, false);
}

void SearchInput::findPrevious()
{
    find(d.lineEdit->text(), false, true, false);
}

void SearchInput::find()
{
    animateShow();
    d.lineEdit->setFocus(Qt::ShortcutFocusReason);
    d.lineEdit->selectAll();
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
        animateShow();
    d.textBrowser->setTextCursor(newCursor);
    d.textBrowser->setExtraSelections(extraSelections);

    d.lineEdit->setObjectName(error ? "error" : "");
    QString css = d.lineEdit->styleSheet();
    d.lineEdit->setStyleSheet(QString());
    d.lineEdit->setStyleSheet(css);
}

void SearchInput::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    if (d.textBrowser) {
        QTextCursor cursor = d.textBrowser->textCursor();
        if (cursor.hasSelection()) {
            cursor.clearSelection();
            d.textBrowser->setTextCursor(cursor);
        }
        d.textBrowser->setExtraSelections(QList<QTextEdit::ExtraSelection>());
        d.textBrowser->buddy()->setFocus();
    }
}

void SearchInput::paintEvent(QPaintEvent*)
{
    QStylePainter painter(this);
    QStyleOption option;
    option.init(this);
    painter.drawPrimitive(QStyle::PE_Widget, option);
}

void SearchInput::animateShow()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "offset");
    animation->setDuration(50);
    animation->setEndValue(-1);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    setVisible(true);
}

void SearchInput::animateHide()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "offset");
    animation->setDuration(50);
    animation->setEndValue(-height());
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation, SIGNAL(destroyed()), this, SLOT(hide()));
}

void SearchInput::reset()
{
    d.lineEdit->clear();
    d.lineEdit->setObjectName("");
    QString css = d.lineEdit->styleSheet();
    d.lineEdit->setStyleSheet(QString());
    d.lineEdit->setStyleSheet(css);

    setOffset(-height());
    setVisible(false);
}
