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
