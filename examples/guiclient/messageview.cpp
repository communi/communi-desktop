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

#include "messageview.h"
#include "irc.h"

MessageView::MessageView(const QString& receiver, QWidget* parent)
    : QTextBrowser(parent), mReceiver(receiver)
{
}

const QString& MessageView::receiver() const
{
    return mReceiver;
}

bool MessageView::matches(const QString& receiver) const
{
    return !mReceiver.compare(receiver, Qt::CaseInsensitive);
}

void MessageView::setReceiver(const QString& receiver)
{
    mReceiver = receiver;
}

const QStringList& MessageView::nicks() const
{
    return mNicks;
}

bool MessageView::contains(const QString& nick) const
{
    return mNicks.contains(nick, Qt::CaseInsensitive);
}

void MessageView::addNick(const QString& nick)
{
    if (!mNicks.contains(nick, Qt::CaseInsensitive))
        mNicks.append(nick);
}

void MessageView::removeNick(const QString& nick)
{
    if (!mNicks.contains(nick, Qt::CaseInsensitive))
        mNicks.append(nick);
}

void MessageView::receiveMessage(const QString& sender, const QString& message)
{
    logMessage(sender, "<%1> %2", message);
}

void MessageView::receiveNotice(const QString& sender, const QString& message)
{
    logMessage(sender, "[%1] %2", message);
}

void MessageView::receiveAction(const QString& sender, const QString& message)
{
    logMessage(sender, "* %1 %2", message);
}

void MessageView::logMessage(const QString& sender, const QString& format, const QString& message)
{
    append(format.arg(sender).arg(Irc::colorStripFromMirc(message)));
}
