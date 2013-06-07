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

#include "commandparser.h"

CommandParser::CommandParser(QObject* parent) : IrcCommandParser(parent)
{
    connect(this, SIGNAL(commandsChanged(QStringList)), this, SIGNAL(availableCommandsChanged()));

    addCommand(IrcCommand::CtcpAction, "ACTION <target> <message...>");
    addCommand(IrcCommand::Admin, "ADMIN (<server>)");
    addCommand(IrcCommand::Away, "AWAY (<reason...>)");
    addCommand(IrcCommand::Info, "INFO (<server>)");
    addCommand(IrcCommand::Invite, "INVITE <user> (<#channel>)");
    addCommand(IrcCommand::Join, "JOIN <#channel> (<key>)");
    addCommand(IrcCommand::Kick, "KICK (<#channel>) <user> (<reason...>)");
    addCommand(IrcCommand::Knock, "KNOCK <#channel> (<message...>)");
    addCommand(IrcCommand::List, "LIST (<channels>) (<server>)");
    addCommand(IrcCommand::CtcpAction, "ME [target] <message...>");
    addCommand(IrcCommand::Mode, "MODE (<channel/user>) (<mode>) (<arg>)");
    addCommand(IrcCommand::Motd, "MOTD (<server>)");
    addCommand(IrcCommand::Names, "NAMES (<#channel>)");
    addCommand(IrcCommand::Nick, "NICK <nick>");
    addCommand(IrcCommand::Notice, "NOTICE <#channel/user> <message...>");
    addCommand(IrcCommand::Part, "PART (<#channel>) (<message...>)");
    addCommand(IrcCommand::Ping, "PING (<user>)");
    addCommand(IrcCommand::Quit, "QUIT (<message...>)");
    addCommand(IrcCommand::Quote, "QUOTE <command> (<parameters...>)");
    addCommand(IrcCommand::Stats, "STATS <query> (<server>)");
    addCommand(IrcCommand::Time, "TIME (<user>)");
    addCommand(IrcCommand::Topic, "TOPIC (<#channel>) (<topic...>)");
    addCommand(IrcCommand::Trace, "TRACE (<target>)");
    addCommand(IrcCommand::Users, "USERS (<server>)");
    addCommand(IrcCommand::Version, "VERSION (<user>)");
    addCommand(IrcCommand::Who, "WHO <user>");
    addCommand(IrcCommand::Whois, "WHOIS <user>");
    addCommand(IrcCommand::Whowas, "WHOWAS <user>");

    addCommand(IrcCommand::Custom, "CLEAR");
    addCommand(IrcCommand::Custom, "QUERY <user>");
    addCommand(IrcCommand::Custom, "MSG <user/channel> <message...>");
}

QStringList CommandParser::availableCommands() const
{
    QStringList commands = IrcCommandParser::commands();
    commands += d.aliases.keys();
    qSort(commands);
    return commands;
}

QString CommandParser::visualSyntax(const QString& command) const
{
    if (d.aliases.contains(command))
        return expandAlias(command);
    QString syntax = IrcCommandParser::syntax(command);
    return syntax.replace(QRegExp("\\[[^\\]]+\\]"), "").simplified();
}

QStringList CommandParser::suggestedCommands(const QString& command, const QStringList& params) const
{
    QStringList suggestions;
    foreach (const QString& available, availableCommands()) {
        if (!command.compare(available, Qt::CaseInsensitive) || (params.isEmpty() && available.startsWith(command, Qt::CaseInsensitive)))
            suggestions += available;
    }
    return suggestions;
}

QMap<QString, QString> CommandParser::aliases() const
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

IrcCommand* CommandParser::parseCommand(const QString& input) const
{
    return IrcCommandParser::parse(expandAlias(input));
}

QString CommandParser::expandAlias(const QString& input) const
{
    QStringList words = input.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QString alias = words.takeFirst().toUpper();
    if (d.aliases.contains(alias)) {
        int pos = 0;
        QRegExp regExp("\\$(\\d+)");
        QString command = d.aliases.value(alias);
        while ((pos = regExp.indexIn(command)) != -1)
        {
            int index = regExp.cap(1).toInt();
            command.replace(pos, regExp.matchedLength(), words.value(index - 1));
        }
        command.replace("$*", words.join(" "));
        command.replace("$?", currentTarget());
        return command;
    }
    return input;
}
