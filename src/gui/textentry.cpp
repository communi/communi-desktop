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

#include "textentry.h"
#include "completer.h"
#include "commandparser.h"
#include <QStringListModel>
#include <IrcBufferModel>
#include <IrcConnection>
#include <IrcUserModel>
#include <IrcMessage>
#include <IrcChannel>
#include <QShortcut>

TextEntry::TextEntry(QWidget* parent) : LineEditor(parent)
{
    d.buffer = 0;

    d.parser = new CommandParser(this);
    d.parser->setTriggers(QStringList("/"));
    d.parser->setTolerant(true);

    d.userModel = new IrcUserModel(this);
    d.userModel->setSortMethod(Irc::SortByActivity);
    d.userModel->setDynamicSort(true);

    d.completer = new Completer(this);
    d.completer->setEditor(this);
    d.completer->setUserModel(d.userModel);

    QStringList commands;
    foreach (const QString& command, d.parser->availableCommands()) {
        foreach (const QString& trigger, d.parser->triggers())
            commands += trigger + command;
    }
    d.completer->setCommandModel(new QStringListModel(commands, this));

    setAttribute(Qt::WA_MacShowFocusRect, false);

    QShortcut* shortcut = new QShortcut(Qt::Key_Tab, this);
    connect(shortcut, SIGNAL(activated()), d.completer, SLOT(onTabPressed()));

    setButtonVisible(Left, true);
    setAutoHideButton(Left, true);
    setButtonPixmap(Left, QPixmap(":/icons/buttons/tab.png"));
    connect(this, SIGNAL(leftButtonClicked()), d.completer, SLOT(onTabPressed()));

    setButtonVisible(Right, true);
    setAutoHideButton(Right, true);
    setButtonPixmap(Right, QPixmap(":/icons/buttons/return.png"));
    connect(this, SIGNAL(rightButtonClicked()), this, SLOT(sendInput()));

    connect(this, SIGNAL(returnPressed()), this, SLOT(sendInput()));
}

IrcBuffer* TextEntry::currentBuffer() const
{
    return d.buffer;
}

void TextEntry::setCurrentBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        if (d.buffer) {
            d.histories.insert(d.buffer, history());

            IrcBufferModel* model = d.buffer->model();
            disconnect(model, SIGNAL(channelsChanged(QStringList)), d.parser, SLOT(setChannels(QStringList)));
            disconnect(buffer, SIGNAL(titleChanged(QString)), d.parser, SLOT(setTarget(QString)));
        }

        d.buffer = buffer;
        setHistory(d.histories.value(buffer));

        if (buffer) {
            IrcBufferModel* model = buffer->model();
            connect(model, SIGNAL(channelsChanged(QStringList)), d.parser, SLOT(setChannels(QStringList)));
            connect(buffer, SIGNAL(titleChanged(QString)), d.parser, SLOT(setTarget(QString)));
            connect(buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(cleanup(IrcBuffer*)), Qt::UniqueConnection);

            d.parser->setTarget(buffer->title());
            d.parser->setChannels(buffer->model()->channels());
            d.userModel->setChannel(buffer->toChannel());
            d.completer->setChannelModel(model);
        } else {
            d.parser->reset();
            d.userModel->setChannel(0);
            d.completer->setChannelModel(0);
        }
    }
}

void TextEntry::sendInput()
{
    if (!d.buffer)
        return;

    bool error = false;
    const QStringList lines = text().split(QRegExp("[\\r\\n]"), QString::SkipEmptyParts);
    foreach (const QString& line, lines) {
        if (!line.trimmed().isEmpty()) {
            if (!d.buffer->sendCommand(d.parser->parse(line)))
                error = true;
        }
    }
    if (!error)
        clear();
}

void TextEntry::cleanup(IrcBuffer* buffer)
{
    d.histories.remove(buffer);
}
