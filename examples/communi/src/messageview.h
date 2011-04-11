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
#include <ircreceiver.h>
#include <ircmessage.h>

class QTreeView;
class StringListModel;
struct Settings;

class MessageView : public QWidget, public IrcReceiver
{
    Q_OBJECT
    Q_PROPERTY(QString receiver READ receiver WRITE setReceiver)

public:
    MessageView(IrcSession* session, QWidget* parent = 0);
    ~MessageView();

    QString receiver() const;
    void setReceiver(const QString& receiver);

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

    // IrcReceiver
    virtual void receiveMessage(IrcMessage* message);
    virtual void inviteMessage(IrcInviteMessage*);
    virtual void joinMessage(IrcJoinMessage*);
    virtual void kickMessage(IrcKickMessage*);
    virtual void modeMessage(IrcModeMessage*);
    virtual void nickNameMessage(IrcNickMessage*);
    virtual void noticeMessage(IrcNoticeMessage*);
    virtual void numericMessage(IrcNumericMessage*);
    virtual void partMessage(IrcPartMessage*);
    virtual void privateMessage(IrcPrivateMessage*);
    virtual void quitMessage(IrcQuitMessage*);
    virtual void topicMessage(IrcTopicMessage*);
    virtual void unknownMessage(IrcMessage*);

private slots:
    void onEscPressed();
    void onSend(const QString& text);
    void onAnchorClicked(const QUrl& link);
    void applySettings(const Settings& settings);
    void receiverChanged();

private:
    QString prefixedSender(const QString& sender) const;
    QString senderHtml(const QString& sender) const;
    bool isCurrent() const;

    struct MessageViewData : public Ui::MessageView
    {
        StringListModel* model;
        QString receiver;
        bool timeStamp;
    } d;
};

#endif // MESSAGEVIEW_H
