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

IrcCommand* CommandParser::parseCommand(const QString& receiver, const QString& text, QObject* parent)
{
    if (text.startsWith("/"))
    {
        typedef IrcCommand*(*ParseFunc)(const QString&, const QStringList&, QObject*);

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
            return parseFunc(receiver, words.mid(1), parent);
    }
    else
    {
        return IrcCommand::createMessage(receiver, text, parent);
    }

    // unknown command
    return 0;
}

IrcCommand* CommandParser::parseInvite(const QString& receiver, const QStringList& params, QObject* parent)
{
    // INVITE <user> <channel>
    return 0;
}

IrcCommand* CommandParser::parseJoin(const QString& receiver, const QStringList& params, QObject* parent)
{
    // JOIN <channel> (<key>)
    return 0;
}

IrcCommand* CommandParser::parseKick(const QString& receiver, const QStringList& params, QObject* parent)
{
    // KICK <user> <channel> (<reason>)
    return 0;
}

IrcCommand* CommandParser::parseList(const QString& receiver, const QStringList& params, QObject* parent)
{
    // LIST <channel> (<server>)
    return 0;
}

IrcCommand* CommandParser::parseMe(const QString& receiver, const QStringList& params, QObject* parent)
{
    // ME <message>
    return 0;
}

IrcCommand* CommandParser::parseMode(const QString& receiver, const QStringList& params, QObject* parent)
{
    // MODE <target> <mode> (<arg>) (<mask>)
    return 0;
}

IrcCommand* CommandParser::parseNames(const QString& receiver, const QStringList& params, QObject* parent)
{
    // NAMES <channel>
    return 0;
}

IrcCommand* CommandParser::parseNick(const QString& receiver, const QStringList& params, QObject* parent)
{
    // NICK <nick>
    return 0;
}

IrcCommand* CommandParser::parseNotice(const QString& receiver, const QStringList& params, QObject* parent)
{
    // NOTICE <target> <message>
    return 0;
}

IrcCommand* CommandParser::parsePart(const QString& receiver, const QStringList& params, QObject* parent)
{
    // PART <channel> (<reason>)
    return 0;
}

IrcCommand* CommandParser::parsePing(const QString& receiver, const QStringList& params, QObject* parent)
{
    // PING <target>
    return 0;
}

IrcCommand* CommandParser::parseQuit(const QString& receiver, const QStringList& params, QObject* parent)
{
    // QUIT (<message>)
    return 0;
}

IrcCommand* CommandParser::parseTopic(const QString& receiver, const QStringList& params, QObject* parent)
{
    // TOPIC <channel> (<topic>)
    return 0;
}

IrcCommand* CommandParser::parseWho(const QString& receiver, const QStringList& params, QObject* parent)
{
    // WHO <user>
    return 0;
}

IrcCommand* CommandParser::parseWhois(const QString& receiver, const QStringList& params, QObject* parent)
{
    // WHOIS <user>
    return 0;
}

IrcCommand* CommandParser::parseWhowas(const QString& receiver, const QStringList& params, QObject* parent)
{
    // WHOWAS <user>
    return 0;
}
