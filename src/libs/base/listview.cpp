/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "listview.h"
#include <QContextMenuEvent>
#include <IrcUserModel>
#include <QFontMetrics>
#include <QScrollBar>
#include <IrcCommand>
#include <IrcChannel>
#include <QAction>
#include <QMenu>
#include <Irc>

ListView::ListView(QWidget* parent) : QListView(parent)
{
    setFocusPolicy(Qt::NoFocus);

    d.model = new IrcUserModel(this);
    d.model->setSortMethod(Irc::SortByTitle);
    setModel(d.model);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
}

IrcChannel* ListView::channel() const
{
    return d.model->channel();
}

void ListView::setChannel(IrcChannel* channel)
{
    if (d.model->channel() != channel) {
        d.model->setChannel(channel);
        emit channelChanged(channel);
    }
}

QSize ListView::sizeHint() const
{
    const int w = 16 * fontMetrics().width('#') + verticalScrollBar()->sizeHint().width();
    return QSize(w, QListView::sizeHint().height());
}

void ListView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        QMenu* menu = createContextMenu(index);
        menu->exec(event->globalPos());
        delete menu;
    }
    event->accept();
}

void ListView::onDoubleClicked(const QModelIndex& index)
{
    if (index.isValid())
        emit queried(index.data(Irc::NameRole).toString());
}

void ListView::onWhoisTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        IrcCommand* command = IrcCommand::createWhois(action->data().toString());
        channel()->sendCommand(command);
    }
}

void ListView::onQueryTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
        emit queried(action->data().toString());
}

void ListView::onModeTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QStringList params = action->data().toStringList();
        IrcCommand* command = IrcCommand::createMode(channel()->title(), params.at(1), params.at(0));
        channel()->sendCommand(command);
    }
}

void ListView::onKickTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        IrcCommand* command = IrcCommand::createKick(channel()->title(), action->data().toString());
        channel()->sendCommand(command);
    }
}

void ListView::onBanTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        IrcCommand* command = IrcCommand::createMode(channel()->title(), "+b", action->data().toString() + "!*@*");
        channel()->sendCommand(command);
    }
}

QMenu* ListView::createContextMenu(const QModelIndex& index)
{
    const QString name = index.data(Irc::NameRole).toString();
    const QString prefix = index.data(Irc::PrefixRole).toString();

    QMenu* menu = new QMenu(this);

    QAction* whoisAction = menu->addAction(tr("Whois"), this, SLOT(onWhoisTriggered()));
    QAction* queryAction = menu->addAction(tr("Query"), this, SLOT(onQueryTriggered()));
    menu->addSeparator();
    QAction* opAction = menu->addAction(tr("Op"), this, SLOT(onModeTriggered()));
    QAction* voiceAction = menu->addAction(tr("Voice"), this, SLOT(onModeTriggered()));
    menu->addSeparator();
    QAction* kickAction = menu->addAction(tr("Kick"), this, SLOT(onKickTriggered()));
    QAction* banAction = menu->addAction(tr("Ban"), this, SLOT(onBanTriggered()));

    whoisAction->setData(name);
    queryAction->setData(name);
    kickAction->setData(name);
    banAction->setData(name);

    if (prefix.contains("@")) {
        opAction->setText(tr("Deop"));
        opAction->setData(QStringList() << name << "-o");
    } else {
        opAction->setText(tr("Op"));
        opAction->setData(QStringList() << name << "+o");
    }

    if (prefix.contains("+")) {
        voiceAction->setText(tr("Devoice"));
        voiceAction->setData(QStringList() << name << "-v");
    } else {
        voiceAction->setText(tr("Voice"));
        voiceAction->setData(QStringList() << name << "+v");
    }
    return menu;
}
