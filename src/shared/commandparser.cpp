/*
* Copyright (C) 2008-2013 Communi authors
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

#include "commandparser.h"
#include <IrcCommand>
#include <QDateTime>
#include <QHash>
#include <QMap>

CommandParser::Private CommandParser::d;

static QMap<QString, QString>& command_syntaxes()
{
    static QMap<QString, QString> syntaxes;
    if (syntaxes.isEmpty()) {
        syntaxes.insert("ADMIN", "(<server>)");
        syntaxes.insert("AWAY", "(<reason...>)");
        syntaxes.insert("INFO", "(<server>)");
        syntaxes.insert("INVITE", "<user>");
        syntaxes.insert("JOIN", "<channel> (<key>)");
        syntaxes.insert("KICK", "<user> (<reason...>)");
        syntaxes.insert("KNOCK", "<channel> (<message...>)");
        syntaxes.insert("LIST", "(<channels>) (<server>)");
        syntaxes.insert("ME", "<message...>");
        syntaxes.insert("MODE", "<channel/user> (<mode>) (<arg>)");
        syntaxes.insert("MOTD", "(<server>)");
        syntaxes.insert("NAMES", "(<channel>)");
        syntaxes.insert("NICK", "<nick>");
        syntaxes.insert("NOTICE", "<channel/user> <message...>");
        syntaxes.insert("PART", "(<reason...>)");
        syntaxes.insert("PING", "(<user>)");
        syntaxes.insert("QUIT" , "(<message...>)");
        syntaxes.insert("QUOTE" , "<command> (<parameters...>)");
        syntaxes.insert("STATS", "<query> (<server>)");
        syntaxes.insert("TIME", "(<user>)");
        syntaxes.insert("TOPIC", "(<topic...>)");
        syntaxes.insert("TRACE", "(<target>)");
        syntaxes.insert("USERS", "(<server>)");
        syntaxes.insert("VERSION", "(<user>)");
        syntaxes.insert("WHO", "<user>");
        syntaxes.insert("WHOIS", "<user>");
        syntaxes.insert("WHOWAS", "<user>");
    }
    return syntaxes;
}

static QString expandAlias(const QString& receiver, const QString& message)
{
    QStringList words = message.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QString alias = words.takeFirst().toUpper();
    QMap<QString, QString> aliases = CommandParser::aliases();
    if (aliases.contains(alias)) {
        int pos = 0;
        QRegExp regExp("\\$(\\d+)");
        QString command = aliases.value(alias);
        while ((pos = regExp.indexIn(command)) != -1)
        {
            int index = regExp.cap(1).toInt();
            command.replace(pos, regExp.matchedLength(), words.value(index - 1));
        }
        command.replace("$*", words.join(" "));
        command.replace("$?", receiver);
        return command;
    }
    return message;
}

QStringList CommandParser::availableCommands()
{
    QStringList commands = command_syntaxes().keys() + d.aliases.keys();
    qSort(commands);
    return commands;
}

QStringList CommandParser::suggestedCommands(const QString& command, const QStringList& params)
{
    QStringList suggestions;
    foreach (const QString& available, availableCommands()) {
        if (!command.compare(available, Qt::CaseInsensitive) || (params.isEmpty() && available.startsWith(command, Qt::CaseInsensitive)))
            suggestions += available;
    }
    return suggestions;
}

QString CommandParser::syntax(const QString& command)
{
    QString cmd = command.toUpper();
    if (command_syntaxes().contains(cmd))
        return cmd + " " + command_syntaxes().value(cmd);
    if (d.aliases.contains(cmd))
        return d.aliases.value(cmd);
    return QString();
}

void CommandParser::addCustomCommand(const QString& command, const QString& syntax)
{
    command_syntaxes().insert(command.toUpper(), syntax);
}

void CommandParser::removeCustomCommand(const QString& command)
{
    command_syntaxes().remove(command.toUpper());
}

QMap<QString, QString> CommandParser::aliases()
{
    return d.aliases;
}

void CommandParser::setAliases(const QMap<QString, QString>& aliases)
{
    d.aliases.clear();
    QMapIterator<QString, QString> it(aliases);
    while (it.hasNext()) {
        it.next();
        d.aliases[it.key().toUpper()] = it.value();
    }
}

IrcCommand* CommandParser::parseCommand(const QString& receiver, const QString& text)
{
    if (text.startsWith("//") || text.startsWith("/ ") || !text.startsWith('/')) {
        QString message = text;
        if (message.startsWith('/'))
            message.remove(0, 1);
        return IrcCommand::createMessage(receiver, message.trimmed());
    } else {
        typedef IrcCommand*(*ParseFunc)(const QString&, const QStringList&);

        static QHash<QString, ParseFunc> parseFunctions;
        if (parseFunctions.isEmpty()) {
            parseFunctions.insert("ADMIN", &CommandParser::parseAdmin);
            parseFunctions.insert("AWAY", &CommandParser::parseAway);
            parseFunctions.insert("INFO", &CommandParser::parseInfo);
            parseFunctions.insert("INVITE", &CommandParser::parseInvite);
            parseFunctions.insert("JOIN", &CommandParser::parseJoin);
            parseFunctions.insert("KICK", &CommandParser::parseKick);
            parseFunctions.insert("KNOCK", &CommandParser::parseKnock);
            parseFunctions.insert("LIST", &CommandParser::parseList);
            parseFunctions.insert("ME", &CommandParser::parseMe);
            parseFunctions.insert("MODE", &CommandParser::parseMode);
            parseFunctions.insert("MOTD", &CommandParser::parseMotd);
            parseFunctions.insert("NAMES", &CommandParser::parseNames);
            parseFunctions.insert("NICK", &CommandParser::parseNick);
            parseFunctions.insert("NOTICE", &CommandParser::parseNotice);
            parseFunctions.insert("PART", &CommandParser::parsePart);
            parseFunctions.insert("PING", &CommandParser::parsePing);
            parseFunctions.insert("QUIT", &CommandParser::parseQuit);
            parseFunctions.insert("QUOTE", &CommandParser::parseQuote);
            parseFunctions.insert("STATS", &CommandParser::parseStats);
            parseFunctions.insert("TIME", &CommandParser::parseTime);
            parseFunctions.insert("TOPIC", &CommandParser::parseTopic);
            parseFunctions.insert("TRACE", &CommandParser::parseTrace);
            parseFunctions.insert("USERS", &CommandParser::parseUsers);
            parseFunctions.insert("VERSION", &CommandParser::parseVersion);
            parseFunctions.insert("WHO", &CommandParser::parseWho);
            parseFunctions.insert("WHOIS", &CommandParser::parseWhois);
            parseFunctions.insert("WHOWAS", &CommandParser::parseWhowas);
        }

        const QString expanded = expandAlias(receiver, text.mid(1));
        const QStringList words = expanded.split(" ", QString::SkipEmptyParts);
        const QString command = words.value(0).toUpper();
        ParseFunc parseFunc = parseFunctions.value(command);
        if (parseFunc) {
            IrcCommand* cmd = parseFunc(receiver, words.mid(1));
            if (cmd)
                return cmd;
        } else if (command_syntaxes().contains(command.toUpper())) {
            return parseCustomCommand(command, words.mid(1), command_syntaxes().value(command.toUpper()));
        }
    }

    // unknown command
    return 0;
}

IrcCommand* CommandParser::parseCustomCommand(const QString& command, const QStringList& params, const QString& syntax)
{
    QStringList tokens = syntax.split(" ", QString::SkipEmptyParts);
    int min = 0;
    int max = tokens.count();
    while (!tokens.isEmpty())
    {
        QString p = tokens.takeFirst();
        if (!p.startsWith("(<"))
            ++min;
        if (tokens.isEmpty() && (p.endsWith("...>") || p.endsWith("...>)")))
            max = INT_MAX;
    }
    if (params.count() >= min && params.count() <= max) {
        IrcCommand* cmd = new IrcCommand;
        cmd->setType(IrcCommand::Custom);
        cmd->setParameters(QStringList(command) + params);
        return cmd;
    }
    return 0;
}

IrcCommand* CommandParser::parseAdmin(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    return IrcCommand::createAdmin(params.value(0));
}

IrcCommand* CommandParser::parseAway(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    return IrcCommand::createAway(params.value(0));
}

IrcCommand* CommandParser::parseInfo(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    return IrcCommand::createUsers(params.join(" "));
}

IrcCommand* CommandParser::parseInvite(const QString& receiver, const QStringList& params)
{
    if (params.count() == 1)
        return IrcCommand::createInvite(params.at(0), receiver);
    return 0;
}

IrcCommand* CommandParser::parseJoin(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (params.count() == 1 || params.count() == 2)
        return IrcCommand::createJoin(params.at(0), params.value(1));
    return 0;
}

IrcCommand* CommandParser::parseKick(const QString& receiver, const QStringList& params)
{
    if (params.count() >= 1)
        return IrcCommand::createKick(receiver, params.at(0), QStringList(params.mid(1)).join(" "));
    return 0;
}

IrcCommand* CommandParser::parseKnock(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (params.count() >= 1)
        return IrcCommand::createKnock(params.at(0), QStringList(params.mid(1)).join(" "));
    return 0;
}

IrcCommand* CommandParser::parseList(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    return IrcCommand::createList(params);
}

IrcCommand* CommandParser::parseMe(const QString& receiver, const QStringList& params)
{
    if (!params.isEmpty())
        return IrcCommand::createCtcpAction(receiver, params.join(" "));
    return 0;
}

IrcCommand* CommandParser::parseMode(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (params.count() >= 1 && params.count() <= 4)
        return IrcCommand::createMode(params.at(0), params.value(1), params.value(2));
    return 0;
}

IrcCommand* CommandParser::parseMotd(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    return IrcCommand::createMotd(params.join(" "));
}

IrcCommand* CommandParser::parseNames(const QString& receiver, const QStringList& params)
{
    if (params.isEmpty())
        return IrcCommand::createNames(receiver);
    return IrcCommand::createNames(params);
}

IrcCommand* CommandParser::parseNick(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (params.count() == 1)
        return IrcCommand::createNick(params.at(0));
    return 0;
}

IrcCommand* CommandParser::parseNotice(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (params.count() >= 2)
        return IrcCommand::createNotice(params.at(0), QStringList(params.mid(1)).join(" "));
    return 0;
}

IrcCommand* CommandParser::parsePart(const QString& receiver, const QStringList& params)
{
    return IrcCommand::createPart(receiver, params.join(" "));
}

IrcCommand* CommandParser::parsePing(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    QString time = QString::number(QDateTime::currentDateTime().toTime_t());
    if (params.isEmpty())
        return IrcCommand::createQuote(QStringList() << "PING" << time);
    return IrcCommand::createCtcpRequest(params.at(0), "PING " + time);
}

IrcCommand* CommandParser::parseQuit(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    return IrcCommand::createQuit(params.join(" "));
}

IrcCommand* CommandParser::parseQuote(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    return IrcCommand::createQuote(params);
}

IrcCommand* CommandParser::parseStats(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (!params.isEmpty())
        return IrcCommand::createStats(params.join(" "));
    return 0;
}

IrcCommand* CommandParser::parseTime(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (params.isEmpty())
        return IrcCommand::createTime();
    return IrcCommand::createCtcpRequest(params.at(0), "TIME");
}

IrcCommand* CommandParser::parseTopic(const QString& receiver, const QStringList& params)
{
    return IrcCommand::createTopic(receiver, params.join(" "));
}

IrcCommand* CommandParser::parseTrace(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    return IrcCommand::createTrace(params.join(" "));
}

IrcCommand* CommandParser::parseUsers(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    return IrcCommand::createUsers(params.join(" "));
}

IrcCommand* CommandParser::parseVersion(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (params.isEmpty())
        return IrcCommand::createVersion();
    return IrcCommand::createCtcpRequest(params.at(0), "VERSION");
}

IrcCommand* CommandParser::parseWho(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (params.count() == 1)
        return IrcCommand::createWho(params.at(0));
    return 0;
}

IrcCommand* CommandParser::parseWhois(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (params.count() == 1)
        return IrcCommand::createWhois(params.at(0));
    return 0;
}

IrcCommand* CommandParser::parseWhowas(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (params.count() == 1)
        return IrcCommand::createWhowas(params.at(0));
    return 0;
}

