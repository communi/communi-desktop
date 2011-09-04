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

#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

#include "ui_messageview.h"
#include <ircsession.h>
#include <ircmessage.h>
#include "commandparser.h"
#include "messageformatter.h"

struct Settings;
class QStringListModel;

class MessageView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString receiver READ receiver WRITE setReceiver)

public:
    MessageView(IrcSession* session, QWidget* parent = 0);
    ~MessageView();

    QString receiver() const;
    void setReceiver(const QString& receiver);

public slots:
    void showHelp(const QString& text, bool error = false);
    void appendMessage(const QString& message);

signals:
    void highlight(MessageView* view, bool on);
    void alert(MessageView* view, bool on);
    void rename(MessageView* view);
    void query(const QString& user);

protected:
    bool eventFilter(QObject* receiver, QEvent* event);

protected slots:
    void receiveMessage(IrcMessage* message);

private slots:
    void onEscPressed();
    void onSend(const QString& text);
    void applySettings(const Settings& settings);

private:
    bool isChannelView() const;

    struct MessageViewData : public Ui::MessageView
    {
        QString receiver;
        IrcSession* session;
        CommandParser parser;
        MessageFormatter formatter;
        QStringListModel* userModel;
        static QStringListModel* commandModel;
    } d;
};

#endif // MESSAGEVIEW_H
