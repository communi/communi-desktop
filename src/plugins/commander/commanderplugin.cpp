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

#include "commanderplugin.h"
#include "treewidget.h"
#include "bufferview.h"
#include "splitview.h"
#include "textinput.h"
#include "listview.h"
#include "textbrowser.h"
#include <QCoreApplication>
#include <IrcCommandParser>
#include <IrcBufferModel>
#include <IrcConnection>
#include <IrcCommand>
#include <IrcChannel>

CommanderPlugin::CommanderPlugin(QObject* parent) : QObject(parent)
{
    d.view = 0;
    d.tree = 0;
}

void CommanderPlugin::initialize(BufferView* view)
{
    IrcCommandParser* parser = view->textInput()->parser();
    parser->addCommand(IrcCommand::Custom, "CLEAR");
    parser->addCommand(IrcCommand::Custom, "CLOSE");
    parser->addCommand(IrcCommand::Custom, "MSG <user/channel> <message...>");
    parser->addCommand(IrcCommand::Custom, "QUERY <user> (<message...>)");
}

void CommanderPlugin::initialize(SplitView* view)
{
    d.view = view;
}

void CommanderPlugin::initialize(TreeWidget* tree)
{
    d.tree = tree;
}

void CommanderPlugin::initialize(IrcConnection* connection)
{
    connection->installCommandFilter(this);
}

void CommanderPlugin::uninitialize(IrcConnection* connection)
{
    connection->removeCommandFilter(this);
}

bool CommanderPlugin::commandFilter(IrcCommand* command)
{
    if (command->type() == IrcCommand::Custom) {
        const QString cmd = command->parameters().value(0);
        const QStringList params = command->parameters().mid(1);
        if (cmd == "CLEAR") {
            d.view->currentView()->textBrowser()->clear();
            return true;
        } else if (cmd == "CLOSE") {
            IrcBuffer* buffer = d.view->currentBuffer();
            IrcChannel* channel = buffer->toChannel();
            if (channel)
                channel->part(qApp->property("description").toString());
            buffer->deleteLater();
            return true;
        } else if (cmd == "MSG") {
            const QString target = params.value(0);
            const QString message = QStringList(params.mid(1)).join(" ");
            if (!message.isEmpty()) {
                IrcBuffer* buffer = d.view->currentBuffer()->model()->add(target);
                IrcCommand* command = IrcCommand::createMessage(target, message);
                if (buffer->sendCommand(command)) {
                    IrcConnection* connection = buffer->connection();
                    buffer->receiveMessage(command->toMessage(connection->nickName(), connection));
                }
                d.tree->setCurrentBuffer(buffer);
                return true;
            }
        } else if (cmd == "QUERY") {
            const QString target = params.value(0);
            const QString message = QStringList(params.mid(1)).join(" ");
            IrcBuffer* buffer = d.view->currentBuffer()->model()->add(target);
            if (!message.isEmpty()) {
                IrcCommand* command = IrcCommand::createMessage(target, message);
                if (buffer->sendCommand(command)) {
                    IrcConnection* connection = buffer->connection();
                    buffer->receiveMessage(command->toMessage(connection->nickName(), connection));
                }
            }
            d.tree->setCurrentBuffer(buffer);
            return true;
        }
    }
    return false;
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(CommanderPlugin)
#endif
