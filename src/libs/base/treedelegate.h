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

#ifndef TREEDELEGATE_H
#define TREEDELEGATE_H

#include <QStyledItemDelegate>

class TreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    Q_PROPERTY(bool dark READ isDark WRITE setDark)
    Q_PROPERTY(bool rootIsDecorated READ rootIsDecorated WRITE setRootIsDecorated)

public:
    explicit TreeDelegate(QObject* parent = 0);

    bool isDark() const;
    void setDark(bool dark);

    bool rootIsDecorated() const;
    void setRootIsDecorated(bool decorated);

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    struct Private {
        bool dark;
        bool rootIsDecorated;
    } d;
};

#endif // TREEDELEGATE_H
