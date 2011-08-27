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

CommandParser::CommandParser(QObject* parent) : QObject(parent)
{
}

CommandParser::~CommandParser()
{
}

IrcCommand* CommandParser::parseCommand(const QString& receiver, const QString& text)
{
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
        ParseFunc parseFunc = parseFunctions.value(words.value(0).toUpper());
        if (parseFunc)
            return parseFunc(receiver, words.mid(1));
    }
    else
    {
        return IrcCommand::createMessage(receiver, text);
    }

    // unknown command
    return 0;
}

IrcCommand* CommandParser::parseInvite(const QString& receiver, const QStringList& params)
{
    // INVITE <user>
    if (params.count() == 1)
        return IrcCommand::createInvite(params.at(0), receiver);
    return 0;
}

IrcCommand* CommandParser::parseJoin(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    // JOIN <channel> (<key>)
    if (params.count() == 1 || params.count() == 2)
        return IrcCommand::createJoin(params.at(0), params.value(1));
    return 0;
}

IrcCommand* CommandParser::parseKick(const QString& receiver, const QStringList& params)
{
    // KICK <user> (<reason>)
    if (params.count() >= 1)
        return IrcCommand::createKick(params.at(0), receiver, QStringList(params.mid(1)).join(" "));
    return 0;
}

IrcCommand* CommandParser::parseList(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    // LIST <channel> (<server>)
    if (params.count() == 1 || params.count() == 2)
        return IrcCommand::createList(params.at(0), params.value(1));
    return 0;
}

IrcCommand* CommandParser::parseMe(const QString& receiver, const QStringList& params)
{
    // ME <message>
    if (!params.isEmpty())
        return IrcCommand::createCtcpAction(receiver, params.join(" "));
    return 0;
}

IrcCommand* CommandParser::parseMode(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    // MODE <target> <mode> (<arg>) (<mask>)
    if (params.count() >= 2 && params.count() <= 4)
        return IrcCommand::createMode(params.at(0), params.at(1), params.value(2), params.value(3));
    return 0;
}

IrcCommand* CommandParser::parseNames(const QString& receiver, const QStringList& params)
{
    // NAMES
    if (params.isEmpty())
        return IrcCommand::createNames(receiver);
    return 0;
}

IrcCommand* CommandParser::parseNick(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    // NICK <nick>
    if (params.count() == 1)
        return IrcCommand::createNick(params.at(0));
    return 0;
}

IrcCommand* CommandParser::parseNotice(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    // NOTICE <target> <message>
    if (params.count() >= 2)
        return IrcCommand::createNotice(params.at(0), QStringList(params.mid(1)).join(" "));
    return 0;
}

IrcCommand* CommandParser::parsePart(const QString& receiver, const QStringList& params)
{
    // PART (<reason>)
    return IrcCommand::createPart(receiver, params.join(" "));
}

IrcCommand* CommandParser::parsePing(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    // PING <target>
    if (params.count() == 1)
        return IrcCommand::createPing(params.at(0));
    return 0;
}

IrcCommand* CommandParser::parseQuit(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    // QUIT (<message>)
    return IrcCommand::createQuit(params.join(" "));
}

IrcCommand* CommandParser::parseTopic(const QString& receiver, const QStringList& params)
{
    // TOPIC (<topic>)
    return IrcCommand::createTopic(receiver, params.join(" "));
}

IrcCommand* CommandParser::parseWho(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    // WHO <user>
    if (params.count() == 1)
        return IrcCommand::createWho(params.at(0));
    return 0;
}

IrcCommand* CommandParser::parseWhois(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    // WHOIS <user>
    if (params.count() == 1)
        return IrcCommand::createWhois(params.at(0));
    return 0;
}

IrcCommand* CommandParser::parseWhowas(const QString& receiver, const QStringList& params)
{
    Q_UNUSED(receiver);
    // WHOWAS <user>
    if (params.count() == 1)
        return IrcCommand::createWhowas(params.at(0));
    return 0;
}
