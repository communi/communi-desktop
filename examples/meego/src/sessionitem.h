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

#ifndef SESSIONITEM_H
#define SESSIONITEM_H

#include "abstractsessionitem.h"
#include "messagehandler.h"
class IrcSession;

class SessionItem : public AbstractSessionItem
{
    Q_OBJECT
    Q_PROPERTY(QObjectList childItems READ childItems NOTIFY childItemsChanged)

public:
    explicit SessionItem(Session* session);

    QObjectList childItems() const;
    QStringList channels() const;

    void updateCurrent(AbstractSessionItem* item);

public slots:
    QObject* addChild(const QString& name);
    void renameChild(const QString& from, const QString& to);
    void removeChild(const QString& name);
    void quit();

signals:
    void childItemsChanged();

protected slots:
    virtual void receiveMessage(IrcMessage* message);

private slots:
    void updateState();

private:
    bool m_closing;
    QObjectList m_children;
    MessageHandler m_handler;
};

#endif // SESSIONITEM_H
