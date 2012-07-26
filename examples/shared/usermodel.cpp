/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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

#include "usermodel.h"
#include "session.h"

class NameLessThan
{
public:
    NameLessThan(const QString& channel) : pfx(channel) { }

    inline bool operator()(const QString &n1, const QString &n2) const
    {
        const int i1 = pfx.indexOf(n1.at(0));
        const int i2 = pfx.indexOf(n2.at(0));

        if (i1 >= 0 && i2 < 0)
            return true;
        if (i1 < 0 && i2 >= 0)
            return false;
        if (i1 >= 0 && i2 >= 0 && i1 != i2)
            return i1 < i2;

        return QString::localeAwareCompare(n1.toLower(), n2.toLower()) < 0;
    }

private:
    QString pfx;
};

UserModel::UserModel(Session* parent) : QAbstractListModel(parent)
{
}

UserModel::~UserModel()
{
}

void UserModel::addUser(const QString& user)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    d.names += user;
    endInsertRows();
}

void UserModel::removeUser(const QString& user)
{
    int idx = d.names.indexOf(user);
    beginRemoveRows(QModelIndex(), idx, idx);
    d.names.removeAt(idx);
    endRemoveRows();
}

int UserModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return d.names.count();
}

QVariant UserModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= d.names.count())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return d.names.at(index.row());

    return QVariant();
}
