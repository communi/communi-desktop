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

class QTreeView;
class StringListModel;
struct Settings;

class MessageView : public QWidget
{
    Q_OBJECT

public:
    MessageView(/* TODO: Irc::Buffer* buffer,*/ QWidget* parent = 0);
    ~MessageView();

    //TODO: Irc::Buffer* buffer() const;
    //TODO: void setBuffer(Irc::Buffer* buffer);

public slots:
    void clear();
    void showHelp(const QString& command = QString());
    void receiveMessage(const QString& format, const QStringList& params, bool highlight = false);

signals:
    void send(const QString& receiver, const QString& text);
    void query(const QString& nick);
    void highlight(MessageView* view, bool on);
    void alert(MessageView* view, bool on);
    void rename(MessageView* view);

protected:
    bool eventFilter(QObject* receiver, QEvent* event);

private slots:
    void onEscPressed();
    void onSend(const QString& text);
    void onAnchorClicked(const QUrl& link);
    void applySettings(const Settings& settings);
    void receiverChanged();

    void msgJoined(const QString& origin);
    void msgParted(const QString& origin, const QString& message);
    void msgQuit(const QString& origin, const QString& message);
    void msgNickChanged(const QString& origin, const QString& nick);
    void msgModeChanged(const QString& origin, const QString& mode, const QString& args);
    void msgTopicChanged(const QString& origin, const QString& topic);
    void msgInvited(const QString& origin, const QString& receiver, const QString& channel);
    void msgKicked(const QString& origin, const QString& nick, const QString& message);
    void msgMessageReceived(const QString& origin, const QString& message);
    void msgNoticeReceived(const QString& origin, const QString& notice);
    void msgCtcpRequestReceived(const QString& origin, const QString& request);
    void msgCtcpReplyReceived(const QString& origin, const QString& reply);
    void msgCtcpActionReceived(const QString& origin, const QString& action);
    void msgNumericMessageReceived(const QString& origin, uint code, const QStringList& params);
    void msgUnknownMessageReceived(const QString& origin, const QStringList& params);

private:
    QString prefixedSender(const QString& sender) const;
    QString senderHtml(const QString& sender) const;

    struct MessageViewData : public Ui::MessageView
    {
        //TODO: Irc::Buffer* buffer;
        StringListModel* model;
        bool timeStamp;
    } d;
};

#endif // MESSAGEVIEW_H
