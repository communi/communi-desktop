/*
  Copyright (C) 2008-2014 The Communi Project

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

#include "abstractfinder.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QStylePainter>
#include <QStyleOption>
#include <QApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QEvent>

AbstractFinder::AbstractFinder(QWidget* parent) : QWidget(parent)
{
    d.offset = -1;
    d.error = false;

    parent->installEventFilter(this);
    setGraphicsEffect(new QGraphicsOpacityEffect(this));

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

AbstractFinder::~AbstractFinder()
{
    emit destroyed(this);
}

QString AbstractFinder::text() const
{
    return d.lineEdit->text();
}

void AbstractFinder::setText(const QString& text)
{
    d.lineEdit->setText(text);
}

int AbstractFinder::offset() const
{
    return d.offset;
}

void AbstractFinder::setOffset(int offset)
{
    d.offset = offset;
    relocate();
}

bool AbstractFinder::hasError() const
{
    return d.error;
}

void AbstractFinder::setError(bool error)
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

bool AbstractFinder::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::Resize)
        relocate();
    return false;
}

void AbstractFinder::findNext()
{
    find(d.lineEdit->text(), true, false, false);
}

void AbstractFinder::findPrevious()
{
    find(d.lineEdit->text(), false, true, false);
}

void AbstractFinder::doFind()
{
    setFixedHeight(d.lineEdit->sizeHint().height() + 1);
    setOffset(-sizeHint().height());
    relocate();
    animateShow();
    reFind();
}

void AbstractFinder::reFind()
{
    d.lineEdit->setFocus(Qt::ShortcutFocusReason);
    d.lineEdit->selectAll();
}

void AbstractFinder::animateShow()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "offset");
    animation->setDuration(50);
    animation->setEndValue(-1);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    setVisible(true);
}

void AbstractFinder::animateHide()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "offset");
    animation->setDuration(50);
    animation->setEndValue(-sizeHint().height());
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    connect(animation, SIGNAL(destroyed()), this, SLOT(hide()));
    connect(animation, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}
