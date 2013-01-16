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

#ifndef SESSIONTREEDELEGATE_H
#define SESSIONTREEDELEGATE_H

#include <QStyledItemDelegate>

class SessionTreeItem;
class SessionTreeWidget;

class SessionTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit SessionTreeDelegate(SessionTreeWidget* parent = 0);

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

signals:
    void closeRequested(SessionTreeItem* item);

protected:
    bool eventFilter(QObject* object, QEvent* event);

private:
    QPersistentModelIndex pressedIndex;
};

#endif // SESSIONTREEDELEGATE_H
