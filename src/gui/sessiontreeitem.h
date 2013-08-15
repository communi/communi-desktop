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

#ifndef SESSIONTREEITEM_H
#define SESSIONTREEITEM_H

#include <QTreeWidgetItem>

class IrcConnection;
class MessageView;

class SessionTreeItem : public QTreeWidgetItem
{
public:
    SessionTreeItem(MessageView* view, QTreeWidget* parent);
    SessionTreeItem(MessageView* view, QTreeWidgetItem* parent);
    ~SessionTreeItem();

    IrcConnection* connection() const;
    MessageView* view() const;
    SessionTreeItem* findChild(const QString& name) const;

    QVariant data(int column, int role) const;

    int badge() const;
    void setBadge(int badget);

    bool isHighlighted() const;
    void setHighlighted(bool highlighted);

    enum SortOrder { Alphabetic, Manual };
    void sort(SortOrder order);
    SortOrder currentSortOrder() const;

    QStringList manualSortOrder() const;
    void setManualSortOrder(const QStringList& order);
    void resetManualSortOrder();

    bool operator<(const QTreeWidgetItem& other) const;

private:
    struct Private {
        bool highlighted;
        MessageView* view;
        SortOrder sortOrder;
        QStringList manualOrder;
        QSet<SessionTreeItem*> highlightedChildren;
    } d;
    friend class SessionTreeWidget;
};

#endif // SESSIONTREEITEM_H
