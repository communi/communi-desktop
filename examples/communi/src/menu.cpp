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

#include "menu.h"
#include <QDateTime>

Menu::Menu(QWidget* parent) : QMenu(parent)
{
}

Menu* Menu::createNickContextMenu(const QString& nick, const QString& channel, QWidget* parent)
{
    Menu* menu = new Menu(parent);
    QAction* action = 0;

    action = menu->addAction(Menu::tr("&Whois"), menu, SLOT(onActionTriggered()));
    action->setData(QString("WHOIS %1 %1").arg(nick));

    action = menu->addAction(Menu::tr("&Version"), menu, SLOT(onActionTriggered()));
    action->setData(QString("PRIVMSG %1 :\x01VERSION\x01").arg(nick));

    action = menu->addAction(Menu::tr("&Ping"), menu, SLOT(onActionTriggered()));
    action->setData(QString("PRIVMSG %1 :\x01PING %2\x01").arg(nick).arg(QDateTime::currentDateTime().toTime_t()));

    menu->addSeparator();
    QMenu* modeMenu = menu->addMenu("&Mode");

    action = modeMenu->addAction(Menu::tr("&Op"), menu, SLOT(onActionTriggered()));
    action->setData(QString("MODE %2 +o %1").arg(nick).arg(channel));

    action = modeMenu->addAction(Menu::tr("&Deop"), menu, SLOT(onActionTriggered()));
    action->setData(QString("MODE %2 -o %1").arg(nick).arg(channel));

    action = modeMenu->addAction(Menu::tr("&Voice"), menu, SLOT(onActionTriggered()));
    action->setData(QString("MODE %2 +v %1").arg(nick).arg(channel));

    action = modeMenu->addAction(Menu::tr("D&evoice"), menu, SLOT(onActionTriggered()));
    action->setData(QString("MODE %2 -v %1").arg(nick).arg(channel));

    QMenu* kickMenu = menu->addMenu("&Kick / Ban");

    action = kickMenu->addAction(Menu::tr("&Kick"), menu, SLOT(onActionTriggered()));
    action->setData(QString("KICK %2 %1").arg(nick).arg(channel));

    action = kickMenu->addAction(Menu::tr("&Ban"), menu, SLOT(onActionTriggered()));
    action->setData(QString("MODE %2 +b %1").arg(nick).arg(channel));

    return menu;
}

void Menu::onActionTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);
    emit command(action->data().toString());
}
