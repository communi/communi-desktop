/*
* Copyright (C) 2008 J-P Nurmi jpnurmi@gmail.com
* Copyright (C) 2008 Adam Higerd ahigerd@libqxt.org
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* $Id$
*/

#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

#include <QTextBrowser>

class MessageView : public QTextBrowser
{
    Q_OBJECT

public:
    MessageView(const QString& receiver, QWidget* parent = 0);

    const QString& receiver() const;
    bool matches(const QString& receiver) const;
    void setReceiver(const QString& receiver);

    const QStringList& nicks() const;
    bool contains(const QString& nick) const;
    void addNick(const QString& nick);
    void removeNick(const QString& nick);

    void receiveNicks(const QStringList& nicks);
    void receiveMessage(const QString& sender, const QString& message, bool highlight = false);
    void receiveNotice(const QString& sender, const QString& message, bool highlight = false);
    void receiveAction(const QString& sender, const QString& message, bool highlight = false);
    void logMessage(const QString& sender, const QString& format, const QString& message, bool highlight = false);

private:
    QString mReceiver;
    QStringList mNicks;
};

#endif // MESSAGEVIEW_H
