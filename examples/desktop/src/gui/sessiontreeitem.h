/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
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

class Session;
class MessageView;

class SessionTreeItem : public QTreeWidgetItem
{
public:
    SessionTreeItem(MessageView* view, QTreeWidget* parent);
    SessionTreeItem(MessageView* view, QTreeWidgetItem* parent);
    ~SessionTreeItem();

    Session* session() const;
    MessageView* view() const;
    SessionTreeItem* findChild(const QString& name) const;

    QVariant data(int column, int role) const;

    bool isAlerted() const;
    void setAlerted(bool alerted);

    bool isHighlighted() const;
    void setHighlighted(bool highlighted);

    bool operator<(const QTreeWidgetItem& other) const;

private:
    struct Private {
        bool alerted;
        bool highlighted;
        MessageView* view;
        QSet<SessionTreeItem*> alertedChildren;
        QSet<SessionTreeItem*> highlightedChildren;
    } d;
    friend class SessionTreeWidget;
};

#endif // SESSIONTREEITEM_H
