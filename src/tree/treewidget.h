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

#include <QTreeWidget>
#include <QShortcut>
#include <QHash>

class TreeItem;
class IrcBuffer;
class IrcConnection;

class TreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    TreeWidget(QWidget* parent = 0);

    QSize sizeHint() const;

    QByteArray saveState() const;
    void restoreState(const QByteArray& state);

    IrcBuffer* currentBuffer() const;
    TreeItem* bufferItem(IrcBuffer* buffer) const;

    QList<IrcConnection*> connections() const;

public slots:
    void addBuffer(IrcBuffer* buffer);
    void removeBuffer(IrcBuffer* buffer);
    void setCurrentBuffer(IrcBuffer* buffer);

signals:
    void currentBufferChanged(IrcBuffer* buffer);

protected:
    void rowsAboutToBeRemoved(const QModelIndex & parent, int start, int end);

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
    friend class TreeItem;
};

#endif // TREEWIDGET_H
