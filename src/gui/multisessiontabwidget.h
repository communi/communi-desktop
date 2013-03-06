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

#ifndef MULTISESSIONTABWIDGET_H
#define MULTISESSIONTABWIDGET_H

#include <QStackedWidget>

class Session;
class IrcMessage;
class MessageView;
class SessionTabWidget;

class MultiSessionTabWidget : public QStackedWidget
{
    Q_OBJECT

public:
    MultiSessionTabWidget(QWidget* parent = 0);

    QList<Session*> sessions() const;

    SessionTabWidget* currentWidget() const;
    SessionTabWidget* widgetAt(int index) const;
    SessionTabWidget* sessionWidget(Session* session) const;

    QByteArray saveSplitter() const;

public slots:
    void addSession(Session* session);
    void removeSession(Session* session);

    void restoreSplitter(const QByteArray& state);

signals:
    void splitterChanged(const QByteArray& state);
};

#endif // MULTISESSIONTABWIDGET_H
