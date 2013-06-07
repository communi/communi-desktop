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

#ifndef SORTEDUSERMODEL_H
#define SORTEDUSERMODEL_H

#include <QStringList>
#include <QSortFilterProxyModel>

class IrcUserModel;

class SortedUserModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    SortedUserModel(IrcUserModel* userModel);

    void sortByPrefixes(const QStringList& prefixes);

protected:
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const;

private:
    QStringList m_prefixes;
};

#endif // SORTEDUSERMODEL_H
