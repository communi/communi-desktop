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

#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QHash>
#include <QTreeWidget>

class TreeItem;
class IrcBuffer;
class IrcConnection;

class TreeWidget : public QTreeWidget
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* currentBuffer READ currentBuffer WRITE setCurrentBuffer NOTIFY currentBufferChanged)

public:
    explicit TreeWidget(QWidget* parent = 0);

    IrcBuffer* currentBuffer() const;
    TreeItem* bufferItem(IrcBuffer* buffer) const;

    QList<IrcConnection*> connections() const;

public slots:
    void addBuffer(IrcBuffer* buffer);
    void removeBuffer(IrcBuffer* buffer);
    void setCurrentBuffer(IrcBuffer* buffer);

signals:
    void bufferAdded(IrcBuffer* buffer);
    void bufferRemoved(IrcBuffer* buffer);
    void currentBufferChanged(IrcBuffer* buffer);

protected:
    QSize sizeHint() const;

private slots:
    void onItemExpanded(QTreeWidgetItem* item);
    void onItemCollapsed(QTreeWidgetItem* item);
    void onCurrentItemChanged(QTreeWidgetItem* item);

private:
    struct Private {
        QList<IrcConnection*> connections;
        QHash<IrcBuffer*, TreeItem*> bufferItems;
        QHash<IrcConnection*, TreeItem*> connectionItems;
    } d;
};

#endif // TREEWIDGET_H
