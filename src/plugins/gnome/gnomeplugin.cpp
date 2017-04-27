/*
  Copyright (C) 2008-2016 The Communi Project

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

#include "gnomeplugin.h"
#include "gnomemenu.h"
#include "x11helper.h"
#include "themeinfo.h"
#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QAction>
#include <QDebug>
#include <QGuiApplication>

GnomePlugin::GnomePlugin(QObject* parent) : QObject(parent)
{
    d.mute = 0;
    d.window = 0;
    d.theme = 0;
    d.isActive = (qgetenv("XDG_CURRENT_DESKTOP") == "GNOME");
    d.isRunningX11 = (static_cast<QGuiApplication*>(QCoreApplication::instance())->platformName() == "xcb");
}

void GnomePlugin::windowCreated(QMainWindow* window)
{
    if (!d.isActive)
        return;

    d.window = window;
#ifdef COMMUNI_HAVE_GIO
    GnomeMenu *section1 = new GnomeMenu(window);
    section1->addSimpleItem("showConnect", "Connect...", window, "doConnect");
    section1->addSimpleItem("showSettings", "Settings", window, "showSettings");
    section1->addSimpleItem("showHelp", "Help", window, "showHelp");

    GnomeMenu *section2 = new GnomeMenu(window);
    section2->addToggleItem("toggleMute", "Mute", d.mute->isChecked(), d.mute, "toggle");

    GnomeMenu *section3 = new GnomeMenu(window);
    section3->addSimpleItem("quit", "Quit", window, "close");

    GnomeMenu *builder = new GnomeMenu(window);
    builder->addSection(section1);
    builder->addSection(section2);
    builder->addSection(section3);
    builder->setMenuToWindow(window->winId(), "/org/communi/gnomeintegration");
#endif // COMMUNI_HAVE_GIO
}

void GnomePlugin::windowShowEvent(QMainWindow *, QShowEvent *)
{
    if (!d.isActive)
        return;

    if (d.theme) {
        d.window->createWinId();
        themeChanged(*d.theme);
    }
}

void GnomePlugin::themeChanged(const ThemeInfo& theme)
{
    if (!d.isActive)
        return;

    d.theme = &theme;
    QByteArray gtkThemeVariant = theme.gtkThemeVariant().toUtf8();

    if (d.isRunningX11) {
        X11Helper::setWindowProperty(d.window->winId(), "_GTK_THEME_VARIANT", gtkThemeVariant);
    }
}

void GnomePlugin::setupTrayIcon(QSystemTrayIcon* tray)
{
    if (!d.isActive)
        return;

    if (qgetenv("XDG_CURRENT_DESKTOP") != "KDE" && qgetenv("KDE_FULL_SESSION").isEmpty())
        tray->setVisible(false);
}

void GnomePlugin::setupMuteAction(QAction* action)
{
    if (!d.isActive)
        return;

    d.mute = action;
}
