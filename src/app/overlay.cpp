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

#include "overlay.h"
#include "bufferview.h"
#include "textbrowser.h"
#include <QStyleOptionButton>
#include <QPropertyAnimation>
#include <QCoreApplication>
#include <QStylePainter>
#include <IrcConnection>
#include <QPushButton>
#include <QShortcut>
#include <IrcBuffer>
#include <QPointer>
#include <QStyle>
#include <QEvent>

class OverlayButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int rotation READ rotation WRITE setRotation)
    Q_PROPERTY(bool waiting READ isWaiting WRITE setWaiting NOTIFY waitingChanged)
    Q_PROPERTY(bool connecting READ isConnecting WRITE setConnecting NOTIFY connectingChanged)

public:
    OverlayButton(QWidget* parent) : QPushButton(parent)
    {
        d.rotation = 0;
        d.waiting = false;
        d.connecting = false;
    }

    bool isWaiting() const
    {
        return d.waiting;
    }

    void setWaiting(bool waiting)
    {
        if (d.waiting != waiting) {
            d.waiting = waiting;
            emit waitingChanged();
            refresh();
        }
    }

    bool isConnecting() const
    {
        return d.connecting;
    }

    void setConnecting(bool connecting)
    {
        if (d.connecting != connecting) {
            d.connecting = connecting;
            emit connectingChanged();
            update();
            if (isVisible()) {
                if (connecting)
                    startAnimation();
                else
                    stopAnimation();
            }
        }
    }

    int rotation() const
    {
        return d.rotation;
    }

    void setRotation(int rotation)
    {
        if (d.rotation != rotation) {
            d.rotation = rotation;
            update();
            refresh();
        }
    }

signals:
    void waitingChanged();
    void connectingChanged();

protected:
    void paintEvent(QPaintEvent*)
    {
        QStylePainter painter(this);
        QStyleOptionButton button;
        initStyleOption(&button);
        painter.translate(width() / 2, height() / 2);
        painter.rotate(d.rotation);
        painter.translate(-width() / 2, -height() / 2);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.drawControl(QStyle::CE_PushButton, button);
    }

    void showEvent(QShowEvent *)
    {
        if (d.connecting)
            startAnimation();
    }

    void hideEvent(QHideEvent *)
    {
        if (d.connecting)
            stopAnimation();
    }

private slots:
    void refresh()
    {
        setFlat(d.waiting && (d.rotation == 0 || d.rotation == 360));
    }

    void startAnimation()
    {
        if (!d.animation) {
            d.animation = new QPropertyAnimation(this, "rotation");
            d.animation->setDuration(750);
            d.animation->setStartValue(0);
            d.animation->setEndValue(360);
            d.animation->setLoopCount(-1);
        }
        d.animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    void stopAnimation()
    {
        if (d.animation) {
            QPropertyAnimation* finish = new QPropertyAnimation(this, "rotation");
            finish->setDuration(d.animation->duration() - d.animation->currentLoopTime());
            finish->setStartValue(d.animation->currentValue());
            finish->setEndValue(d.animation->endValue());
            finish->setLoopCount(1);
            finish->start(QAbstractAnimation::DeleteWhenStopped);
            connect(finish, SIGNAL(finished()), this, SLOT(refresh()));

            d.animation->stop();
            delete d.animation;
            d.animation = 0;
        }
    }

private:
    struct Private {
        int rotation;
        bool waiting;
        bool connecting;
        QPointer<QPropertyAnimation> animation;
    } d;
};

Overlay::Overlay(BufferView* view) : QFrame(view)
{
    d.buffer = 0;
    d.view = view;

    d.button = new OverlayButton(view);
    connect(d.button, SIGNAL(clicked()), this, SLOT(toggle()));

    d.shortcut = new QShortcut(Qt::Key_Space, view);
    d.shortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(d.shortcut, SIGNAL(activated()), this, SLOT(toggle()));

    setVisible(false);
    setFocusProxy(d.button);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    view->installEventFilter(this);

    connect(view, SIGNAL(bufferChanged(IrcBuffer*)), this, SLOT(init(IrcBuffer*)));
    init(view->buffer());
}

BufferView* Overlay::view() const
{
    return d.view;
}

bool Overlay::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::Resize)
        relayout();
    return false;
}

void Overlay::refresh()
{
    IrcConnection* connection = d.buffer ? d.buffer->connection() : 0;
    const bool connected = connection && connection->status() == IrcConnection::Connected;
    const bool waiting = connection && connection->status() == IrcConnection::Waiting;
    const bool connecting = connection && connection->status() == IrcConnection::Connecting;
    d.shortcut->setEnabled(!connected && !connecting);
    d.button->setVisible(!connected);
    d.button->setWaiting(waiting);
    d.button->setConnecting(connecting);
    setVisible(!connected);
}

void Overlay::relayout()
{
    resize(parentWidget()->size());
    d.button->setGeometry(QStyle::alignedRect(layoutDirection(), Qt::AlignCenter, d.button->size(), rect()));
}

void Overlay::toggle()
{
    IrcConnection* connection = d.buffer ? d.buffer->connection() : 0;
    if (connection) {
        if (connection->isEnabled()) {
            connection->setEnabled(false);
            connection->quit(qApp->property("description").toString());
        } else {
            connection->setEnabled(true);
            connection->open();
        }
    }
}

void Overlay::init(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        if (d.buffer) {
            IrcConnection* connection = d.buffer->connection();
            disconnect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(refresh()));
        }
        d.buffer = buffer;
        if (buffer) {
            IrcConnection* connection = buffer->connection();
            connect(connection, SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(refresh()));
        }
        refresh();
    }
}

#include "overlay.moc"
