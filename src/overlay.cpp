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

#include "overlay.h"
#include <QResizeEvent>
#include <QMovie>

Overlay::Overlay(QWidget* parent) : QLabel(parent)
{
    d.dark = false;
    d.button = 0;
    d.prevParent = 0;

    d.button = new QToolButton(this);
    d.button->setObjectName("reconnectButton");
    d.button->setFixedSize(32, 32);
    connect(d.button, SIGNAL(clicked()), this, SIGNAL(refresh()));

    setFocusProxy(d.button);
    setFocusPolicy(Qt::StrongFocus);

    setVisible(false);
    setAlignment(Qt::AlignCenter);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

bool Overlay::isBusy() const
{
    return movie();
}

void Overlay::setBusy(bool busy)
{
    QMovie* mov = movie();
    if (busy) {
        if (!mov) {
            mov = new QMovie(QString(":/resources/ajax-loader-%1.gif").arg(d.dark ? "dark" : "light"), QByteArray(), this);
            setMovie(mov);
        }
        mov->start();
    } else {
        delete mov;
    }
}

bool Overlay::isDark() const
{
    return d.dark;
}

void Overlay::setDark(bool dark)
{
    if (d.dark != dark) {
        d.dark = dark;
        if (movie())
            movie()->setFileName(QString(":/resources/ajax-loader-%1.gif").arg(d.dark ? "dark" : "light"));
    }
}

bool Overlay::hasRefresh() const
{
    return d.button;
}

void Overlay::setRefresh(bool enabled)
{
    d.button->setVisible(enabled);
}

bool Overlay::event(QEvent* event)
{
    if (event->type() == QEvent::ParentChange)
        reparent();
    return QLabel::event(event);
}

bool Overlay::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::Resize)
        relayout();
    return false;
}

void Overlay::relayout()
{
    resize(parentWidget()->size());
    if (d.button)
        d.button->move(rect().center() - d.button->rect().center());
}

void Overlay::reparent()
{
    if (d.prevParent)
        d.prevParent->removeEventFilter(this);
    Q_ASSERT(parentWidget());
    parentWidget()->installEventFilter(this);
    d.button->setParent(parentWidget());
    d.prevParent = parentWidget();
    relayout();
}
