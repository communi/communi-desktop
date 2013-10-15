/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QStandardItemModel>

class IrcBuffer;
class IrcBufferModel;

class TreeModel : public QStandardItemModel
{
    Q_OBJECT

public:
    TreeModel(QObject* parent = 0);
    ~TreeModel();

    void addModel(IrcBufferModel* model);
    void removeModel(IrcBufferModel* model);
    QList<IrcBufferModel*> models() const;

    using QStandardItemModel::index;
    QModelIndex index(IrcBuffer* buffer) const;
    IrcBuffer* buffer(const QModelIndex& index) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex&index, const QVariant& value, int role = Qt::EditRole);

private slots:
    void onBufferAdded(IrcBuffer* buffer);
    void onBufferRemoved(IrcBuffer* buffer);

private:
    struct Private {
        QList<IrcBufferModel*> models;
        QHash<IrcBuffer*, QStandardItem*> items;
        QHash<QStandardItem*, IrcBuffer*> buffers;
        QHash<IrcBuffer*, int> badges;
        QHash<IrcBuffer*, bool> highlights;
    } d;
};

#endif // TREEMODEL_H
