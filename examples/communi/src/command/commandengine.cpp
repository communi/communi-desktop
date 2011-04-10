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

#include "commandengine.h"
#include "application.h"
#include <QDebug>

#include <QtScript>
#ifndef QT_NO_DEBUG
#include <QtScriptTools>
#endif // QT_NO_DEBUG

CommandEngine::CommandEngine(QObject* parent) : QScriptEngine(parent)
{
#ifndef QT_NO_DEBUG
    Q_ASSERT(parent);
    // passing 'this' as parent causes a crash at close
    QScriptEngineDebugger* debugger = new QScriptEngineDebugger(parent);
    debugger->attachTo(this);
#endif // QT_NO_DEBUG
}

QObject* CommandEngine::scriptObject(const QString& name) const
{
    return globalObject().property(name).toQObject();
}

void CommandEngine::setScriptObject(const QString& name, QObject* object)
{
    globalObject().setProperty(name, newQObject(object));
}

bool CommandEngine::evaluateCommand(QString message, QString currentReceiver, QStringList allReceivers)
{
    QStringList words = message.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QString alias = words.takeFirst();
    message = words.join(" ");
    QString command = Application::settings().aliases.value(alias, alias);
    if (command != alias)
    {
        QRegExp regExp("\\$(\\d+)");
        int pos = 0;
        while ((pos = regExp.indexIn(command)) != -1)
        {
            int index = regExp.cap(1).toInt();
            command.replace(pos, regExp.matchedLength(), words.value(index - 1));
        }
        command.replace("$*", message);
        command.replace("$?", currentReceiver);
        return evaluateCommand(command, currentReceiver, allReceivers);
    }

    foreach (const Command& cmd, Application::commands())
    {
        if (cmd.name == command)
        {
            globalObject().setProperty("allReceivers", toScriptValue<QStringList>(allReceivers));
            globalObject().setProperty("currentReceiver", toScriptValue<QString>(currentReceiver));
            globalObject().setProperty("message", toScriptValue<QString>(message));
            globalObject().setProperty("words", toScriptValue<QStringList>(words));

            evaluate(cmd.script);

            if (hasUncaughtException())
            {
                qDebug() << uncaughtExceptionBacktrace();
                return false;
            }
            return true;
        }
    }
    return false;
}
