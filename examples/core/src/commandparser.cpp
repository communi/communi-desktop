/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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

#include "commandparser.h"
#include <IrcCommand>
#include <QHash>
#include <QMap>

static QMap<QString, QString> command_syntaxes()
{
    static QMap<QString, QString> syntaxes;
    if (syntaxes.isEmpty())
    {
        syntaxes.insert("INVITE", "INVITE <user>");
        syntaxes.insert("JOIN", "JOIN <channel> (<key>)");
        syntaxes.insert("KICK", "KICK <user> (<reason>)");
        syntaxes.insert("LIST", "LIST <channel> (<server>)");
        syntaxes.insert("ME", "ME <message>");
        syntaxes.insert("MODE", "MODE <target> <mode> (<arg>) (<mask>)");
        syntaxes.insert("NAMES", "NAMES");
        syntaxes.insert("NICK", "NICK <nick>");
        syntaxes.insert("NOTICE", "NOTICE <target> <message>");
        syntaxes.insert("PART", "PART (<reason>)");
        syntaxes.insert("PING", "PING <target>");
        syntaxes.insert("QUERY" , "QUERY (<user>)");
        syntaxes.insert("QUIT" , "QUIT (<message>)");
        syntaxes.insert("TOPIC", "TOPIC (<topic>)");
        syntaxes.insert("WHO", "WHO <user>");
        syntaxes.insert("WHOIS", "WHOIS <user>");
        syntaxes.insert("WHOWAS", "WHOWAS <user>");
    }
    return syntaxes;
}

CommandParser::CommandParser(QObject* parent) : QObject(parent)
{
}

CommandParser::~CommandParser()
{
}

QStringList CommandParser::availableCommands()
{
    return command_syntaxes().keys();
}

QString CommandParser::suggestedCommand(const QString& command)
{
    const QStringList commands = availableCommands();
    for (int i = commands.size() - 1; i >= 0; --i)
        if (commands.at(i).startsWith(command, Qt::CaseInsensitive))
            return commands.at(i);
    return QString();
}

QString CommandParser::syntax(const QString& command)
{
    return command_syntaxes().value(command.toUpper());
}

Q_GLOBAL_STATIC(QStringList, custom_commands)

QStringList CommandParser::customCommands()
{
    return *custom_commands();
}

void CommandParser::setCustomCommands(const QStringList& commands)
{
    *custom_commands() = commands;
}

Q_GLOBAL_STATIC(bool, has_error)

bool CommandParser::hasError() const
{
    return *has_error();
}

IrcCommand* CommandParser::parseCommand(const QString& receiver, const QString& text)
{
    *has_error() = false;
    if (text.startsWith("/"))
    {
        typedef IrcCommand*(*ParseFunc)(const QString&, const QStringList&);

        static QHash<QString, ParseFunc> parseFunctions;
        if (parseFunctions.isEmpty())
        {
            parseFunctions.insert("INVITE", &CommandParser::parseInvite);
            parseFunctions.insert("JOIN", &CommandParser::parseJoin);
            parseFunctions.insert("KICK", &CommandParser::parseKick);
            parseFunctions.insert("LIST", &CommandParser::parseList);
            parseFunctions.insert("ME", &CommandParser::parseMe);
            parseFunctions.insert("MODE", &CommandParser::parseMode);
            parseFunctions.insert("NAMES", &CommandParser::parseNames);
            parseFunctions.insert("NICK", &CommandParser::parseNick);
            parseFunctions.insert("NOTICE", &CommandParser::parseNotice);
            parseFunctions.insert("PART", &CommandParser::parsePart);
            parseFunctions.insert("PING", &CommandParser::parsePing);
            parseFunctions.insert("QUIT", &CommandParser::parseQuit);
            parseFunctions.insert("TOPIC", &CommandParser::parseTopic);
            parseFunctions.insert("WHO", &CommandParser::parseWho);
            parseFunctions.insert("WHOIS", &CommandParser::parseWhois);
            parseFunctions.insert("WHOWAS", &CommandParser::parseWhowas);
        }

        const QStringList words = text.mid(1).split(" ", QString::SkipEmptyParts);
        const QString command = words.value(0).toUpper();
        ParseFunc parseFunc = parseFunctions.value(command);
        if (parseFunc)
            return parseFunc(receiver, words.mid(1));
        if (custom_commands()->contains(command, Qt::CaseInsensitive))
        {
            emit customCommand(command, words.mid(1));
            return 0;
        }
    }
    else
    {
        return IrcCommand::createMessage(receiver, text);
    }

    // unknown command
    *has_error() = true;
    return 0;
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
        return IrcCommand::createKick(params.at(0), receiver, QStringList(params.mid(1)).join(" "));
    return 0;
}

IrcCommand* CommandParser::parseList(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    if (params.count() == 1 || params.count() == 2)
        return IrcCommand::createList(params.at(0), params.value(1));
    return 0;
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
    if (params.count() >= 2 && params.count() <= 4)
        return IrcCommand::createMode(params.at(0), params.at(1), params.value(2), params.value(3));
    return 0;
}

IrcCommand* CommandParser::parseNames(const QString& receiver, const QStringList& params)
{
    if (params.isEmpty())
        return IrcCommand::createNames(receiver);
    return 0;
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
    if (params.count() == 1)
        return IrcCommand::createPing(params.at(0));
    return 0;
}

IrcCommand* CommandParser::parseQuit(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    return IrcCommand::createQuit(params.join(" "));
}

IrcCommand* CommandParser::parseTopic(const QString& receiver, const QStringList& params)
{
    return IrcCommand::createTopic(receiver, params.join(" "));
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
