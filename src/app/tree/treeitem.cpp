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

#include "treeitem.h"
#include "treerole.h"
#include "treewidget.h"
#include "treespinner.h"
#include "treeindicator.h"
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
    if (column == 0 && role == Qt::DisplayRole && d.buffer)
        return d.buffer->title();
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
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
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
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
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
