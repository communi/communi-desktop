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

#ifndef USERMODEL_H
#define USERMODEL_H

#include <QStringList>
#include <QAbstractListModel>

class Session;

class UserModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit UserModel(Session* session);
    virtual ~UserModel();

    bool hasUser(const QString& user) const;
    void addUser(const QString& user);
    void addUsers(const QStringList& users);
    void removeUser(const QString& user);
    void renameUser(const QString& from, const QString& to);
    void setUserMode(const QString& user, const QString& mode);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
    struct Private
    {
        Session* session;
        QStringList names;
        QHash<QString, QString> modes;
    } d;
};

#endif // USERMODEL_H
