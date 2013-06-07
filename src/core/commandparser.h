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

#include <IrcCommandParser>
#include <QString>
#include <QMap>

class CommandParser : public IrcCommandParser
{
    Q_OBJECT
    Q_PROPERTY(QStringList availableCommands READ availableCommands NOTIFY availableCommandsChanged)

public:
    CommandParser(QObject* parent = 0);

    QStringList availableCommands() const;
    Q_INVOKABLE QString visualSyntax(const QString& command) const;
    Q_INVOKABLE QStringList suggestedCommands(const QString& command, const QStringList& params) const;

    QMap<QString, QString> aliases() const;
    void setAliases(const QMap<QString, QString>& aliases);

    Q_INVOKABLE IrcCommand* parseCommand(const QString& input) const;

signals:
    void availableCommandsChanged();

protected:
    QString expandAlias(const QString& input) const;

private:
    struct Private {
        QMap<QString, QString> aliases;
    } d;
};

#endif // COMMANDPARSER_H
