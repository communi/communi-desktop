/*
* Copyright (C) 2008-2014 The Communi Project
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

#include "finder.h"
#include <QPropertyAnimation>
#include <QStylePainter>
#include <QStyleOption>
#include <QApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QEvent>

Finder::Finder(QWidget* parent) : QWidget(parent)
{
    d.offset = -1;
    d.error = false;

    parent->installEventFilter(this);

    d.lineEdit = new QLineEdit(this);
    d.lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
    d.lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    d.prevButton = new QToolButton(this);
    d.prevButton->setObjectName("prev");
    d.prevButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    connect(d.prevButton, SIGNAL(clicked()), this, SLOT(findPrevious()));

    d.nextButton = new QToolButton(this);
    d.nextButton->setObjectName("next");
    d.nextButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    connect(d.nextButton, SIGNAL(clicked()), this, SLOT(findNext()));

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(d.lineEdit);
    layout->addWidget(d.prevButton);
    layout->addWidget(d.nextButton);
    layout->setSpacing(0);
    layout->setMargin(0);

    connect(d.lineEdit, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
    connect(d.lineEdit, SIGNAL(textEdited(QString)), this, SLOT(find(QString)));
}

Finder::~Finder()
{
    emit destroyed(this);
}

int Finder::offset() const
{
    return d.offset;
}

void Finder::setOffset(int offset)
{
    d.offset = offset;
    relocate();
}

bool Finder::hasError() const
{
    return d.error;
}

void Finder::setError(bool error)
{
    if (d.error != error) {
        d.error = error;
        // force restyle
        setStyleSheet(QString());
        d.lineEdit->setStyleSheet(QString());
        d.nextButton->setEnabled(!error);
        d.prevButton->setEnabled(!error);
    }
}

bool Finder::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::Resize)
        relocate();
    return false;
}

void Finder::findNext()
{
    find(d.lineEdit->text(), true, false, false);
}

void Finder::findPrevious()
{
    find(d.lineEdit->text(), false, true, false);
}

void Finder::doFind()
{
    setFixedHeight(d.lineEdit->sizeHint().height() + 1);
    setOffset(-sizeHint().height());
    relocate();
    animateShow();
    reFind();
}

void Finder::reFind()
{
    d.lineEdit->setFocus(Qt::ShortcutFocusReason);
    d.lineEdit->selectAll();
}

void Finder::animateShow()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "offset");
    animation->setDuration(50);
    animation->setEndValue(-1);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    setVisible(true);
}

void Finder::animateHide()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "offset");
    animation->setDuration(50);
    animation->setEndValue(-sizeHint().height());
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation, SIGNAL(destroyed()), this, SLOT(hide()));
    connect(animation, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}
