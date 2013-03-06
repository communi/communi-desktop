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

#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include <QMap>
#include <QString>
#include <QStringList>

class IrcCommand;

class CommandParser
{
public:
    static QStringList availableCommands();
    static QStringList suggestedCommands(const QString& command, const QStringList& params);
    static QString syntax(const QString& command);

    static void addCustomCommand(const QString& command, const QString& syntax);
    static void removeCustomCommand(const QString& command);

    static QMap<QString, QString> aliases();
    static void setAliases(const QMap<QString, QString>& aliases);

    static IrcCommand* parseCommand(const QString& receiver, const QString& text);

private:
    static IrcCommand* parseCustomCommand(const QString& command, const QStringList& params, const QString& syntax);
    static IrcCommand* parseAdmin(const QString& receiver, const QStringList& params);
    static IrcCommand* parseAway(const QString& receiver, const QStringList& params);
    static IrcCommand* parseInfo(const QString& receiver, const QStringList& params);
    static IrcCommand* parseInvite(const QString& receiver, const QStringList& params);
    static IrcCommand* parseJoin(const QString& receiver, const QStringList& params);
    static IrcCommand* parseKick(const QString& receiver, const QStringList& params);
    static IrcCommand* parseKnock(const QString& receiver, const QStringList& params);
    static IrcCommand* parseList(const QString& receiver, const QStringList& params);
    static IrcCommand* parseMe(const QString& receiver, const QStringList& params);
    static IrcCommand* parseMode(const QString& receiver, const QStringList& params);
    static IrcCommand* parseMotd(const QString& receiver, const QStringList& params);
    static IrcCommand* parseNames(const QString& receiver, const QStringList& params);
    static IrcCommand* parseNick(const QString& receiver, const QStringList& params);
    static IrcCommand* parseNotice(const QString& receiver, const QStringList& params);
    static IrcCommand* parsePart(const QString& receiver, const QStringList& params);
    static IrcCommand* parsePing(const QString& receiver, const QStringList& params);
    static IrcCommand* parseQuit(const QString& receiver, const QStringList& params);
    static IrcCommand* parseQuote(const QString& receiver, const QStringList& params);
    static IrcCommand* parseStats(const QString& receiver, const QStringList& params);
    static IrcCommand* parseTime(const QString& receiver, const QStringList& params);
    static IrcCommand* parseTopic(const QString& receiver, const QStringList& params);
    static IrcCommand* parseTrace(const QString& receiver, const QStringList& params);
    static IrcCommand* parseUsers(const QString& receiver, const QStringList& params);
    static IrcCommand* parseVersion(const QString& receiver, const QStringList& params);
    static IrcCommand* parseWho(const QString& receiver, const QStringList& params);
    static IrcCommand* parseWhois(const QString& receiver, const QStringList& params);
    static IrcCommand* parseWhowas(const QString& receiver, const QStringList& params);

    static struct Private {
        QMap<QString, QString> aliases;
    } d;
};

#endif // COMMANDPARSER_H
