/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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

#ifndef SESSIONTABWIDGET_H
#define SESSIONTABWIDGET_H

#include "tabwidget.h"
#include "messagehandler.h"
#include <QHash>

class Session;
class MessageView;
struct Connection;
class IrcMessage;
struct Settings;

class SessionTabWidget : public TabWidget
{
    Q_OBJECT

public:
    SessionTabWidget(Session* session, QWidget* parent = 0);

    Session* session() const;

public slots:
    MessageView* openView(const QString& receiver);
    void removeView(const QString& receiver);
    void closeCurrentView();
    void renameView(const QString& from, const QString& to);
    void quit(const QString& message = QString());

signals:
    void titleChanged(const QString& title);

private slots:
    void onAboutToQuit();
    void onDisconnected();
    void tabActivated(int index);
    void onNewTabRequested();
    void delayedTabReset();
    void delayedTabResetTimeout();
    void alertTab(MessageView* view, bool on);
    void highlightTab(MessageView* view, bool on);
    void applySettings(const Settings& settings);

private:
    struct SessionTabWidgetData
    {
        bool hasQuit;
        QList<int> delayedIndexes;
        MessageHandler handler;
        QHash<QString, MessageView*> views;
    } d;
};

#endif // SESSIONTABWIDGET_H
