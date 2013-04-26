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

#include "usermodel.h"
#include "session.h"
#include <ircmessage.h>
#include <ircsender.h>
#include <irc.h>

UserModel::UserModel(QObject* parent) : QAbstractListModel(parent)
{
    d.session = qobject_cast<Session*>(parent);
}

UserModel::~UserModel()
{
}

Session* UserModel::session() const
{
    return d.session;
}

void UserModel::setSession(Session* session)
{
    d.session = session;
    clearUsers();
}

QString UserModel::channel() const
{
    return d.channel;
}

void UserModel::setChannel(const QString& channel)
{
    d.channel = channel;
}

QStringList UserModel::users() const
{
    return d.names;
}

bool UserModel::hasUser(const QString& user) const
{
    return d.names.contains(user, Qt::CaseInsensitive);
}

void UserModel::addUser(const QString& user)
{
    addUsers(QStringList() << user);
}

void UserModel::addUsers(const QStringList& users)
{
    QHash<QString, QString> unique;
    QSet<QString> nameSet = d.names.toSet();
    foreach (const QString& user, users) {
        QString name = d.session->unprefixedUser(user);
        if (!nameSet.contains(name))
            unique.insert(name, d.session->userPrefix(user));
    }

    if (!unique.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, unique.count() - 1);
        QHash<QString, QString>::const_iterator it = unique.constBegin();
        while (it != unique.constEnd()) {
            d.names.prepend(it.key());
            d.prefixes.insert(it.key(), it.value());
            ++it;
        }
        endInsertRows();
    }
}

void UserModel::removeUser(const QString& user)
{
    int idx = d.names.indexOf(user);
    if (idx != -1) {
        beginRemoveRows(QModelIndex(), idx, idx);
        d.names.removeAt(idx);
        d.prefixes.remove(user);
        endRemoveRows();
    }
}

void UserModel::clearUsers()
{
    if (!d.names.isEmpty()) {
        beginResetModel();
        d.names.clear();
        endResetModel();
    }
}

void UserModel::renameUser(const QString& from, const QString& to)
{
    int idx = d.names.indexOf(from);
    if (idx != -1) {
        beginRemoveRows(QModelIndex(), idx, idx);
        d.names.removeAt(idx);
        QString prefix = d.prefixes.take(from);
        endRemoveRows();

        beginInsertRows(QModelIndex(), 0, 0);
        d.names.prepend(to);
        d.prefixes[to] = prefix;
        endInsertRows();
    }
}

void UserModel::promoteUser(const QString& user)
{
    int idx = d.names.indexOf(user);
    if (idx > 0) {
        // TODO: fix QCompleter to listen QAIM::rowsMoved()
        //beginMoveRows(QModelIndex(), idx, idx, QModelIndex(), 0);
        //d.names.move(idx, 0);
        //endMoveRows();

        beginRemoveRows(QModelIndex(), idx, idx);
        d.names.removeAt(idx);
        endRemoveRows();

        beginInsertRows(QModelIndex(), 0, 0);
        d.names.prepend(user);
        endInsertRows();
    }
}

void UserModel::setUserMode(const QString& user, const QString& mode)
{
    int idx = d.names.indexOf(user);
    if (idx != -1) {
        bool add = true;
        IrcSessionInfo info(d.session);
        QString updated = d.prefixes.value(user);
        for (int i = 0; i < mode.size(); ++i) {
            QChar c = mode.at(i);
            QString p = info.modeToPrefix(c);
            switch (c.unicode()) {
                case '+':
                    add = true;
                    break;
                case '-':
                    add = false;
                    break;
                default:
                    if (add) {
                        if (!updated.contains(p))
                            updated += p;
                    } else {
                        updated.remove(p);
                    }
                    break;
            }
        }
        d.prefixes[user] = updated;
        emit dataChanged(index(idx, 0), index(idx, 0));
    }
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

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole) {
        QString name = d.names.at(index.row());
        if (role == Qt::DisplayRole)
            return d.prefixes.value(name).left(1) + name;
        if (role == Qt::UserRole)
            return d.prefixes.value(name);
        return name;
    }

    return QVariant();
}

void UserModel::processMessage(IrcMessage* message)
{
    if (!d.session) {
        qWarning() << "UserModel::processMessage(): session is null!";
        return;
    }

    if (message->type() == IrcMessage::Nick) {
        QString from = message->sender().name();
        QString to = static_cast<IrcNickMessage*>(message)->nick();
        renameUser(from, to);
    } else if (message->type() == IrcMessage::Join) {
        if (message->flags() & IrcMessage::Own)
            clearUsers();
        else
            addUser(message->sender().name());
    } else if (message->type() == IrcMessage::Part || message->type() == IrcMessage::Quit) {
        if (message->flags() & IrcMessage::Own)
            clearUsers();
        else
            removeUser(message->sender().name());
    } else if (message->type() == IrcMessage::Kick) {
        QString user = static_cast<IrcKickMessage*>(message)->user();
        if (!user.compare(d.session->nickName(), Qt::CaseInsensitive))
            clearUsers();
        else
            removeUser(user);
    } else if (message->type() == IrcMessage::Mode) {
        IrcModeMessage* modeMsg = static_cast<IrcModeMessage*>(message);
        if (modeMsg->sender().name() != modeMsg->target() && !modeMsg->argument().isEmpty())
            setUserMode(modeMsg->argument(), modeMsg->mode());
    } else if (message->type() == IrcMessage::Numeric) {
        if (static_cast<IrcNumericMessage*>(message)->code() == Irc::RPL_NAMREPLY) {
            int count = message->parameters().count();
            if (!d.channel.isNull() && !d.channel.compare(message->parameters().value(count - 2), Qt::CaseInsensitive)) {
                QString names = message->parameters().value(count - 1);
                addUsers(names.split(" ", QString::SkipEmptyParts));
            }
        }
    } else {
        promoteUser(message->sender().name());
    }
}
