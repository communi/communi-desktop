/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "treemodel.h"
#include "treerole.h"
#include <IrcBufferModel>
#include <IrcBuffer>

static const int COLUMNS = 2;

TreeModel::TreeModel(QObject* parent) : QStandardItemModel(0, COLUMNS, parent)
{
}

TreeModel::~TreeModel()
{
}

void TreeModel::addModel(IrcBufferModel* model)
{
    QStandardItem* item = new QStandardItem(0, COLUMNS);
    IrcBuffer* buffer = model->get(0);
    d.items.insert(buffer, item);
    d.buffers.insert(item, buffer);
    d.models += model;

    appendRow(item);

    connect(model, SIGNAL(added(IrcBuffer*)), this, SLOT(onBufferAdded(IrcBuffer*)));
    connect(model, SIGNAL(removed(IrcBuffer*)), this, SLOT(onBufferRemoved(IrcBuffer*)));
}

void TreeModel::removeModel(IrcBufferModel* model)
{
    disconnect(model, SIGNAL(added(IrcBuffer*)), this, SLOT(onBufferAdded(IrcBuffer*)));
    disconnect(model, SIGNAL(removed(IrcBuffer*)), this, SLOT(onBufferRemoved(IrcBuffer*)));

    IrcBuffer* buffer = model->get(0);
    QStandardItem* item = d.items.take(buffer);
    d.buffers.remove(item);
    delete item;
    d.models.removeOne(model);
}

QList<IrcBufferModel*> TreeModel::models() const
{
    return d.models;
}

QModelIndex TreeModel::index(IrcBuffer* buffer) const
{
    return indexFromItem(d.items.value(buffer));
}

IrcBuffer* TreeModel::buffer(const QModelIndex& index) const
{
    return d.buffers.value(itemFromIndex(index));
}

QVariant TreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case Irc::BufferRole:
        return QVariant::fromValue(buffer(index));
    case BadgeRole:
        return d.badges.value(buffer(index));
    case HighlightRole:
        return d.highlights.value(buffer(index));
    case Qt::DisplayRole:
        if (IrcBuffer* buf = buffer(index))
            return buf->title();
    }
    return QVariant();
}

bool TreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    if (role == BadgeRole || role == HighlightRole) {
        if (IrcBuffer* buf = buffer(index)) {
            if (role == BadgeRole)
                d.badges.insert(buf, value.toInt());
            else
                d.highlights.insert(buf, value.toBool());
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

void TreeModel::onBufferAdded(IrcBuffer* buffer)
{
    QStandardItem* parent = d.items.value(buffer->model()->get(0));
    Q_ASSERT(parent);
    QStandardItem* child = new QStandardItem(0, COLUMNS);
    parent->appendRow(child);
    d.items.insert(buffer, child);
    d.buffers.insert(child, buffer);
}

void TreeModel::onBufferRemoved(IrcBuffer* buffer)
{
    d.badges.remove(buffer);
    d.highlights.remove(buffer);

    QStandardItem* item = d.items.take(buffer);
    d.buffers.remove(item);
    delete item;
}
