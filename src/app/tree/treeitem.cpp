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

#include "treeitem.h"
#include "treerole.h"
#include "treewidget.h"
#include "treespinner.h"
#include "treeindicator.h"
#include "treedelegate.h"
#include <IrcConnection>
#include <IrcLagTimer>
#include <IrcBuffer>
#include <QPainter>
#include <QPixmap>

TreeItem::TreeItem(IrcBuffer* buffer, TreeItem* parent) : QObject(buffer), QTreeWidgetItem(parent)
{
    d.anim = 0;
    d.timer = 0;
    init(buffer);
}

TreeItem::TreeItem(IrcBuffer* buffer, TreeWidget* parent) : QObject(buffer), QTreeWidgetItem(parent)
{
    init(buffer);

    d.anim = new QVariantAnimation(this);
    d.anim->setDuration(750);
    d.anim->setStartValue(0);
    d.anim->setEndValue(360);
    d.anim->setLoopCount(-1);
    connect(d.anim, SIGNAL(valueChanged(QVariant)), this, SLOT(updateIcon()));

    d.timer = new IrcLagTimer(this);
    d.timer->setConnection(buffer->connection());
    connect(d.timer, SIGNAL(lagChanged(qint64)), this, SLOT(updateIcon()));
    connect(buffer->connection(), SIGNAL(statusChanged(IrcConnection::Status)), this, SLOT(onStatusChanged()));
    updateIcon();
}

void TreeItem::init(IrcBuffer* buffer)
{
    d.buffer = buffer;
    setObjectName(buffer->title());
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    connect(buffer, SIGNAL(activeChanged(bool)), this, SLOT(refresh()));
    connect(buffer, SIGNAL(titleChanged(QString)), this, SLOT(refresh()));
    connect(buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(onBufferDestroyed()));
}

TreeItem::~TreeItem()
{
    emit destroyed(this);
    d.buffer = 0;
}

IrcBuffer* TreeItem::buffer() const
{
    return d.buffer;
}

IrcConnection* TreeItem::connection() const
{
    if (d.buffer)
        return d.buffer->connection();
    return 0;
}

TreeItem* TreeItem::parentItem() const
{
    return static_cast<TreeItem*>(QTreeWidgetItem::parent());
}

TreeWidget* TreeItem::treeWidget() const
{
    return static_cast<TreeWidget*>(QTreeWidgetItem::treeWidget());
}

QVariant TreeItem::data(int column, int role) const
{
    if (role == TreeRole::Active)
        return d.buffer->isActive();
    if (column == 0 && role == Qt::DisplayRole && d.buffer) {
        TreeWidget* tree = treeWidget();
        if (!tree || !tree->itemDelegate()->isTransient())
            return d.buffer->title();
        return "";
    }
    return QTreeWidgetItem::data(column, role);
}

void TreeItem::setData(int column, int role, const QVariant& value)
{
    QTreeWidgetItem::setData(column, role, value);
    if (role == TreeRole::Highlight && !parentItem())
        updateIcon();
}

bool TreeItem::operator<(const QTreeWidgetItem& other) const
{
    return treeWidget()->lessThan(this, static_cast<const TreeItem*>(&other));
}

void TreeItem::refresh()
{
    emitDataChanged();
}

void TreeItem::updateIcon()
{
    if (!d.timer || !d.anim)
        return;

    qint64 lag = d.timer->lag();
    setToolTip(0, lag > 0 ? tr("%1ms").arg(lag) : QString());

    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (connection()->isActive() && !connection()->isConnected()) {
#if defined(Q_OS_WIN)
        painter.translate(8, 7);
#elif defined(Q_OS_MAC)
        painter.translate(8, 8);
#else
        painter.translate(8, 10);
#endif
        painter.rotate(d.anim->currentValue().toInt());
        TreeSpinner* spinner = TreeSpinner::instance(treeWidget());
        spinner->render(&painter, QPoint(-8, -8));
    } else {
        TreeIndicator* indicator = TreeIndicator::instance(treeWidget());
        QStyle::State state;
        if (data(0, TreeRole::Highlight).toBool())
            state |= QStyle::State_On;
        if (!connection()->isConnected())
            state |= QStyle::State_Off;
        indicator->setState(state);
        indicator->setLag(lag);
#if defined(Q_OS_WIN)
        indicator->render(&painter, QPoint(4, 3));
#elif defined(Q_OS_MAC)
        indicator->render(&painter, QPoint(4, 4));
#else
        indicator->render(&painter, QPoint(4, 6));
#endif
    }

    setIcon(0, pixmap);
}

void TreeItem::onStatusChanged()
{
    if (connection()->isActive() && !connection()->isConnected()) {
        if (d.anim->state() == QAbstractAnimation::Stopped)
            d.anim->start();
    } else {
        if (d.anim->state() == QAbstractAnimation::Running)
            d.anim->stop();
    }
    updateIcon();
}

void TreeItem::onBufferDestroyed()
{
    d.buffer = 0;
}
