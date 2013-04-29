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
#include <QApplication>
#include <QMovie>

Overlay::Overlay(QWidget* parent) : QLabel(parent)
{
    d.button = 0;
    d.prevParent = 0;
    d.forwarding = false;

    d.button = new QToolButton(this);
    d.button->setObjectName("reconnectButton");
    d.button->setFixedSize(32, 32);
    connect(d.button, SIGNAL(clicked()), this, SIGNAL(refresh()));

    setVisible(false);
    setEnabled(false);
    setAutoFillBackground(true);
    setAlignment(Qt::AlignCenter);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    QPalette pal = palette();
    QColor col = pal.color(QPalette::Window);
    col.setAlpha(100);
    pal.setColor(QPalette::Window, col);
    setPalette(pal);
}

bool Overlay::isBusy() const
{
    return movie();
}

void Overlay::setBusy(bool busy)
{
    if (busy) {
        setMovie(new QMovie(":/resources/ajax-loader.gif", QByteArray(), this));
        movie()->start();
    } else {
        delete movie();
    }
}

bool Overlay::hasRefresh() const
{
    return d.button->isVisible();
}

void Overlay::setRefresh(bool enabled)
{
    d.button->setVisible(enabled);
    if (enabled)
        qApp->installEventFilter(this);
    else
        qApp->removeEventFilter(this);
}

bool Overlay::event(QEvent* event)
{
    if (event->type() == QEvent::ParentChange)
        reparent();
    return QLabel::event(event);
}

bool Overlay::eventFilter(QObject* object, QEvent* event)
{
    if (object == parentWidget() && event->type() == QEvent::Resize) {
        relayout();
    } else if (d.button->isVisible() && (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)) {
        if (!d.forwarding) {
            d.forwarding = true;
            qApp->sendEvent(d.button, event);
            d.forwarding = false;
            return true;
        }
    }
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
