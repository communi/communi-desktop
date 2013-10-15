/*
* Copyright (C) 2008-2013 The Communi Project
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
        command.replace("$?", target());
        return command;
    }
    return input;
}
