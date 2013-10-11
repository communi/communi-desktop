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

#include "messagestackview.h"
#include "settingsmodel.h"
#include "application.h"
#include "zncmanager.h"
#include "connection.h"
#include "completer.h"
#include <ircbuffer.h>
#include <irccommand.h>
#include <irclagtimer.h>
#include <ircbuffermodel.h>

MessageStackView::MessageStackView(IrcConnection* connection, QWidget* parent) : QStackedWidget(parent)
{
    d.connection = connection;

    d.bufferModel = new IrcBufferModel(connection);
    connect(d.bufferModel, SIGNAL(added(IrcBuffer*)), this, SLOT(setBuffer(IrcBuffer*)));
    connect(d.bufferModel, SIGNAL(messageIgnored(IrcMessage*)), &d.handler, SLOT(handleMessage(IrcMessage*)));
    connect(d.bufferModel, SIGNAL(channelsChanged(QStringList)), &d.parser, SLOT(setChannels(QStringList)));

    connection->installMessageFilter(qobject_cast<Connection*>(connection)); // TODO
    d.handler.znc()->setModel(d.bufferModel);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(activateView(int)));

    connect(&d.handler, SIGNAL(viewToBeAdded(QString)), this, SLOT(addView(QString)));
    connect(&d.handler, SIGNAL(viewToBeRemoved(QString)), this, SLOT(removeView(QString)));
    connect(&d.handler, SIGNAL(viewToBeRenamed(QString, QString)), this, SLOT(renameView(QString, QString)));

    d.parser.setTolerant(true);
    d.lagTimer = new IrcLagTimer(d.connection);

    MessageView* view = addView(connection->host());
    d.handler.setDefaultView(view);
    d.handler.setCurrentView(view);
    setCurrentWidget(view);

    applySettings();
    connect(Application::settings(), SIGNAL(changed()), this, SLOT(applySettings()));
}

IrcConnection* MessageStackView::connection() const
{
    return d.connection;
}

CommandParser* MessageStackView::parser() const
{
    return &const_cast<MessageStackView*>(this)->d.parser;
}

QStringListModel* MessageStackView::commandModel() const
{
    return &const_cast<MessageStackView*>(this)->d.commandModel;
}

MessageView* MessageStackView::currentView() const
{
    return qobject_cast<MessageView*>(currentWidget());
}

MessageView* MessageStackView::viewAt(int index) const
{
    return qobject_cast<MessageView*>(widget(index));
}

MessageView* MessageStackView::addView(const QString& receiver)
{
    MessageView* view = d.views.value(receiver.toLower());
    bool channel = !receiver.isEmpty() && d.connection->network()->channelTypes().contains(receiver.at(0));
    if (!view) {
        ViewInfo::Type type = ViewInfo::Server;
        if (!d.views.isEmpty())
            type = channel ? ViewInfo::Channel : ViewInfo::Query;
        view = createView(type, receiver);
    }
    if (channel && !view->isActive())
        openView(receiver);
    return view;
}

void MessageStackView::restoreView(const ViewInfo& view)
{
    createView(static_cast<ViewInfo::Type>(view.type), view.name);
}

MessageView* MessageStackView::createView(ViewInfo::Type type, const QString& receiver)
{
    MessageView* view = new MessageView(type, static_cast<Connection*>(d.connection), this); // TODO
    const IrcNetwork* network = d.connection->network();
    view->completer()->setChannelPrefixes(network->channelTypes().join(""));
    view->completer()->setChannelModel(&d.viewModel);
    view->setReceiver(receiver);
    connect(view, SIGNAL(queried(QString)), this, SLOT(addView(QString)));
    connect(view, SIGNAL(queried(QString)), this, SLOT(openView(QString)));
    connect(view, SIGNAL(messaged(QString,QString)), this, SLOT(sendMessage(QString,QString)));
    connect(view, SIGNAL(renamed(QString,QString)), this, SLOT(renameView(QString,QString)));

    connect(d.lagTimer, SIGNAL(lagChanged(qint64)), view, SLOT(updateLag(qint64)));
    view->updateLag(d.lagTimer->lag());

    d.handler.addView(receiver, view);
    d.views.insert(receiver.toLower(), view);
    view->setBuffer(d.bufferModel->add(receiver));
    view->buffer()->setPersistent(true);
    addWidget(view);
    d.viewModel.setStringList(d.viewModel.stringList() << receiver);
    emit viewAdded(view);
    return view;
}

void MessageStackView::openView(const QString& receiver)
{
    MessageView* view = d.views.value(receiver.toLower());
    if (view)
        setCurrentWidget(view);
}

void MessageStackView::removeView(const QString& receiver)
{
    MessageView* view = d.views.take(receiver.toLower());
    if (view) {
        view->deleteLater();
        QStringList views = d.viewModel.stringList();
        if (views.removeOne(receiver))
            d.viewModel.setStringList(views);
        emit viewRemoved(view);
        d.handler.removeView(view->receiver());
    }
}

void MessageStackView::closeView(int index)
{
    MessageView* view = viewAt(index);
    if (view) {
        if (view->isActive()) {
            if (indexOf(view) == 0)
                static_cast<Connection*>(connection())->quit(); // TODO
            else if (view->viewType() == ViewInfo::Channel)
                d.connection->sendCommand(IrcCommand::createPart(view->receiver()));
        }
        d.handler.removeView(view->receiver());
    }
}

void MessageStackView::renameView(const QString& from, const QString& to)
{
    if (!d.views.contains(to.toLower())) {
        MessageView* view = d.views.take(from.toLower());
        if (view) {
            view->setReceiver(to);
            d.views.insert(to.toLower(), view);
            emit viewRenamed(view);
        }
    } else if (currentView() == d.views.value(from.toLower())) {
        setCurrentWidget(d.views.value(to.toLower()));
    }
}

void MessageStackView::sendMessage(const QString& receiver, const QString& message)
{
    MessageView* view = addView(receiver);
    if (view) {
        setCurrentWidget(view);
        view->sendMessage(message);
    }
}

void MessageStackView::applySettings()
{
    SettingsModel* settings = Application::settings();
    d.handler.znc()->setTimeStampFormat(settings->value("formatting.timeStamp").toString());

    QMap<QString,QString> aliases;
    QVariantMap values = settings->values("aliases.*");
    QMapIterator<QString,QVariant> it(values);
    while (it.hasNext()) {
        it.next();
        aliases[it.key().mid(8).toUpper()] = it.value().toString();
    }
    d.parser.setAliases(aliases);

    QStringList commands;
    foreach (const QString& command, d.parser.availableCommands())
        commands += d.parser.prefix() + command;
    d.commandModel.setStringList(commands);
}

void MessageStackView::activateView(int index)
{
    MessageView* view = viewAt(index);
    if (view && isVisible()) {
        d.handler.setCurrentView(view);
        view->setFocus();
        emit viewActivated(view);
    }
}

void MessageStackView::setBuffer(IrcBuffer* buffer)
{
    MessageView* view = addView(buffer->title());
    if (view) {
        view->setBuffer(buffer);
        buffer->setPersistent(true);
    }
}
