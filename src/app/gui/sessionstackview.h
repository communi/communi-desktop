/*
* Copyright (C) 2008-2014 The Communi Project
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

#ifndef SESSIONSTACKVIEW_H
#define SESSIONSTACKVIEW_H

#include <QHash>
#include <QStackedWidget>

class IrcConnection;
class MessageStackView;

class SessionStackView : public QStackedWidget
{
    Q_OBJECT

public:
    SessionStackView(QWidget* parent = 0);

    QList<IrcConnection*> connections() const;

    MessageStackView* currentWidget() const;
    MessageStackView* connectionWidget(IrcConnection* connection) const;

public slots:
    int addConnection(IrcConnection* connection);
    void removeConnection(IrcConnection* connection);

private:
    struct Private {
        QList<IrcConnection*> connections;
        QHash<IrcConnection*, MessageStackView*> connectionWidgets;
    } d;
};

#endif // SESSIONSTACKVIEW_H
