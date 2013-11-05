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

#ifndef BADGEDELEGATE_H
#define BADGEDELEGATE_H

#include "treedelegate.h"

class BadgeDelegate : public TreeDelegate
{
    Q_OBJECT

public:
    explicit BadgeDelegate(QObject* parent = 0);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

protected:
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const;
};

#endif // BADGEDELEGATE_H
