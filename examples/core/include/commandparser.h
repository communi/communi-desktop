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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include <QObject>

class IrcCommand;

class CommandParser : public QObject
{
    Q_OBJECT

public:
    explicit CommandParser(QObject* parent = 0);
    virtual ~CommandParser();

    static Q_INVOKABLE IrcCommand* parseCommand(const QString& receiver, const QString& text, QObject* parent = 0);

private:
    static IrcCommand* parseInvite(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseJoin(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseKick(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseList(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseMe(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseMode(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseNames(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseNick(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseNotice(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parsePart(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parsePing(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseQuit(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseTopic(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseWho(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseWhois(const QString& receiver, const QStringList& params, QObject* parent = 0);
    static IrcCommand* parseWhowas(const QString& receiver, const QStringList& params, QObject* parent = 0);
};

#endif // COMMANDPARSER_H
