/*
  Copyright (C) 2008-2015 The Communi Project

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

#include "osxplugin.h"
#include <QSystemTrayIcon>
#include <IrcTextFormat>
#include <QMainWindow>
#include <IrcMessage>
#include <QMenuBar>
#include <QMenu>

OsxPlugin::OsxPlugin(QObject* parent) : QObject(parent)
{
    d.tray = 0;
}

void OsxPlugin::windowCreated(QMainWindow* window)
{
    QMenu* menu = new QMenu(window);
    window->menuBar()->addMenu(menu);

    QAction* action = new QAction(tr("Preferences"), window);
    action->setMenuRole(QAction::PreferencesRole);
    connect(action, SIGNAL(triggered()), window, SLOT(showSettings()));
    menu->addAction(action);
}

void OsxPlugin::setupTrayIcon(QSystemTrayIcon* tray)
{
    d.tray = tray;
}

void OsxPlugin::dockAlert(IrcMessage* message)
{
    if (message->type() == IrcMessage::Private) {
        IrcPrivateMessage* pm = static_cast<IrcPrivateMessage*>(message);
        if (pm->isPrivate())
            d.tray->showMessage(tr("Private message from %1").arg(pm->nick()), IrcTextFormat().toPlainText(pm->content()));
        else
            d.tray->showMessage(tr("Message from %1 on %2").arg(pm->nick(), pm->target()), IrcTextFormat().toPlainText(pm->content()));
    } else if (message->type() == IrcMessage::Notice) {
        IrcNoticeMessage* nm = static_cast<IrcNoticeMessage*>(message);
        if (nm->isPrivate())
            d.tray->showMessage(tr("Private notice from %1").arg(nm->nick()), IrcTextFormat().toPlainText(nm->content()));
        else
            d.tray->showMessage(tr("Notice from %1 on %2").arg(nm->nick(), nm->target()), IrcTextFormat().toPlainText(nm->content()));
    }
}
