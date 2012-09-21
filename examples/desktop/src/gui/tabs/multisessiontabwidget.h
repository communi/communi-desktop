/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
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

#ifndef MULTISESSIONTABWIDGET_H
#define MULTISESSIONTABWIDGET_H

#include "tabwidget.h"
#include "settings.h"

class Session;
class QShortcut;
class IrcMessage;
class SessionTabWidget;

class MultiSessionTabWidget : public TabWidget
{
    Q_OBJECT

public:
    MultiSessionTabWidget(QWidget* parent = 0);

    QList<Session*> sessions() const;
    void addSession(Session* session);
    void removeSession(Session* session);

    SessionTabWidget* sessionWidget(Session* session) const;

public slots:
    void applySettings(const Settings& settings);

signals:
    void alerted(IrcMessage* message);
    void highlighted(IrcMessage* message);

protected:
    void tabInserted(int index);
    void tabRemoved(int index);

private slots:
    void onSessionNetworkChanged(const QString& network);
    void setInactive(bool inactive);
    void setAlerted(bool alerted);
    void setHighlighted(bool highlighted);
    void tabActivated(int index);
    void onTabMenuRequested(int index, const QPoint& pos);

private:
    int senderIndex() const;

    struct MainTabWidgetData
    {
        QShortcut* tabUpShortcut;
        QShortcut* tabDownShortcut;
        Settings settings;
    } d;
};

#endif // MULTISESSIONTABWIDGET_H
