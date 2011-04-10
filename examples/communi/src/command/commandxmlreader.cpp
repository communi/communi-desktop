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

#include "commandxmlreader.h"
#include "application.h"
#include <QtDebug>

CommandXmlReader::CommandXmlReader()
{
}

Commands CommandXmlReader::read(QIODevice* device)
{
    setDevice(device);

    Commands commands;
    while (!atEnd())
    {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
        {
            if (name() == "commands" && attributes().value("version") == "1.0")
                commands = commands.unite(readCommands());
            else
                raiseError(Application::tr("The file is not a commands version 1.0 file."));
        }
    }
    if (hasError())
    {
        qDebug() << error() << errorString();
        // TODO: error handling
    }
    return commands;
}

Commands CommandXmlReader::readCommands()
{
    Q_ASSERT(isStartElement() && name() == "commands");

    Commands commands;
    while (!atEnd())
    {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
        {
            if (name() == "command")
            {
                Command command = readCommand();
                commands[command.name] = command;
            }
            else
            {
                readUnknown();
            }
        }
    }
    return commands;
}

Command CommandXmlReader::readCommand()
{
    Q_ASSERT(isStartElement() && name() == "command");

    Command command;
    command.name = attributes().value("name").toString();
    while (!atEnd())
    {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
        {
            if (name() == "script")
                command.script = readElementText();
            else if (name() == "help")
                command.help = readElementText();
            else if (name() == "description")
                command.description = readElementText();
            else if (name() == "categories")
                command.categories = readCategories();
            else
                readUnknown();
        }
    }
    return command;
}

QStringList CommandXmlReader::readCategories()
{
    Q_ASSERT(isStartElement() && name() == "categories");

    QStringList categories;
    while (!atEnd())
    {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
        {
            if (name() == "category")
                categories += readElementText();
            else
                readUnknown();
        }
    }
    return categories;
}

void CommandXmlReader::readUnknown()
{
    Q_ASSERT(isStartElement());

    while (!atEnd())
    {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
            readUnknown();
    }
}
