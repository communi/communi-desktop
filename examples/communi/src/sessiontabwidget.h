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

#ifndef SESSIONTABWIDGET_H
#define SESSIONTABWIDGET_H

#include "tabwidget.h"
#include <QHash>

class Session;
class MessageView;
class CommandEngine;
struct Connection;
class IrcMessage;

class SessionTabWidget : public TabWidget
{
    Q_OBJECT

public:
    SessionTabWidget(Session* session, QWidget* parent = 0);

    Session* session() const;

public slots:
    void openView(const QString& receiver);
    void closeCurrentView();

signals:
    void vibraRequested(bool on);
    void titleChanged(const QString& title);
    void disconnectFrom(const QString& message);

private slots:
    void connected();
    void connecting();
    void disconnected();
    void tabActivated(int index);
    void delayedTabReset();
    void delayedTabResetTimeout();
    void nameTab(MessageView* view);
    void alertTab(MessageView* view, bool on);
    void highlightTab(MessageView* view, bool on);
    void applySettings();
    void onMessageReceived(IrcMessage* message);

private:
    void createView(const QString& receiver);

    struct SessionTabWidgetData
    {
        int connectCounter;
        QList<int> delayedIndexes;
        Session* session;
        CommandEngine* engine;
        QHash<QString, MessageView*> views;
    } d;
};

#endif // SESSIONTABWIDGET_H
