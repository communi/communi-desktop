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

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QObject>
#include <QTreeWidgetItem>

class IrcBuffer;
class TreeWidget;
class IrcConnection;
class IrcBufferModel;

class TreeItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT

public:
    TreeItem(IrcBuffer* buffer, QTreeWidget* parent);
    TreeItem(IrcBuffer* buffer, QTreeWidgetItem* parent);
    ~TreeItem();

    IrcBuffer* buffer() const;
    IrcConnection* connection() const;

    TreeItem* parentItem() const;
    TreeWidget* treeWidget() const;

    QVariant data(int column, int role) const;

    int badge() const;
    void setBadge(int badge);

    bool isHighlighted() const;
    void setHighlighted(bool highlighted);

    bool operator<(const QTreeWidgetItem& other) const;

public slots:
    void reset();
    void refresh();

private slots:
    void blink();

private:
    void init(IrcBuffer* buffer);
    void startBlinking();
    void stopBlinking();

    struct Private {
        int badge;
        bool blink;
        bool highlighted;
        IrcBuffer* buffer;
        QSet<TreeItem*> highlightedChildren;
    } d;
    friend class TreeWidget;
};

#endif // TREEITEM_H
