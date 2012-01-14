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

import QtQuick 1.1
import Communi 1.0
import com.nokia.symbian 1.1

PageStackWindow {
    id: window

    platformInverted: true

    initialPage: MainPage {
        id: page

        tools: ToolBarLayout {
            id: tools
            visible: sheet.status == DialogStatus.Closed
            ToolButton {
                iconSource: "toolbar-add"
                anchors.right: parent.right
                onClicked: SessionModel.length ? menu.open() : connectionDialog.open()
                platformInverted: true
            }
        }

        Menu {
            id: menu
            platformInverted: true
            MenuLayout {
                MenuItem {
                    text: qsTr("Add connection")
                    onClicked: connectionDialog.open()
                    platformInverted: true
                }
                MenuItem {
                    text: qsTr("Join channel")
                    onClicked: channelDialog.open()
                    platformInverted: true
                }
                MenuItem {
                    text: qsTr("Open query")
                    onClicked: chatDialog.open()
                    platformInverted: true
                }
            }
        }

        IrcCommand {
            id: ircCommand
        }

        ConnectionDialog {
            id: connectionDialog

            Component.onCompleted: {
                connectionDialog.host = Settings.host;
                connectionDialog.port = Settings.port;
                connectionDialog.name = Settings.name;
                connectionDialog.channel = Settings.channel;
                connectionDialog.secure = Settings.secure;
            }

            Component {
                id: sessionComponent
                Session { }
            }

            onAccepted: {
                var session = sessionComponent.createObject(window);
                session.nickName = connectionDialog.name;
                session.userName = connectionDialog.name;
                session.realName = connectionDialog.name;
                session.host = connectionDialog.host;
                session.port = connectionDialog.port;
                session.password = connectionDialog.password;
                session.secure = connectionDialog.secure;
                session.channels = connectionDialog.channel;
                SessionManager.addSession(session);

                connectionDialog.password = "";
                Settings.host = connectionDialog.host;
                Settings.port = connectionDialog.port;
                Settings.name = connectionDialog.name;
                Settings.channel = connectionDialog.channel;
                Settings.secure = connectionDialog.secure;
            }
        }

        Connections {
            target: SessionManager
            onChannelKeyRequired: {
                channelDialog.channel = channel;
                channelDialog.passwordRequired = true;
                channelDialog.open();
            }
        }

        ChannelDialog {
            id: channelDialog
            onAccepted: {
                var item = SessionModel[channelDialog.sessionIndex];
                if (item) {
                    var child = item.addChild(channelDialog.channel);
                    var cmd = ircCommand.createJoin(channelDialog.channel, channelDialog.password);
                    page.bouncer.bounce(child, cmd);
                }
            }
        }

        ChatDialog {
            id: chatDialog
            onAccepted: {
                var item = SessionModel[chatDialog.sessionIndex];
                if (item) {
                    var child = item.addChild(chatDialog.name);
                    var cmd = ircCommand.createWhois(chatDialog.name);
                    page.bouncer.bounce(child, cmd);
                }
            }
        }
    }

    Component.onCompleted: SessionManager.restore()
    Component.onDestruction: SessionManager.save()
}
