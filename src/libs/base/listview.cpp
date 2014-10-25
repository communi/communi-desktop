/*
  Copyright (C) 2008-2014 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#ifdef Q_OS_MAC
    setVerticalScrollMode(ScrollPerPixel);
#endif

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
    menu->addAction(name)->setEnabled(false);
    menu->addSeparator();

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
