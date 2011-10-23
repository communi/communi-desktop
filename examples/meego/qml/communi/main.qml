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
import com.nokia.meego 1.0

PageStackWindow {
    id: window

    initialPage: MainPage {
        title: qsTr("Communi")
        tools: ToolBarLayout {
            id: tools
            visible: sheet.status == DialogStatus.Closed
            ToolIcon {
                iconId: "toolbar-add"
                anchors.right: parent.right
                onClicked: SessionModel.length ? menu.open() : connectionSheet.open()
            }
        }

        Menu {
            id: menu
            MenuLayout {
                MenuItem {
                    text: qsTr("Add connection")
                    onClicked: connectionSheet.open()
                }
                MenuItem {
                    text: qsTr("Join channel")
                    onClicked: channelSheet.open()
                }
                MenuItem {
                    text: qsTr("Open query")
                    onClicked: querySheet.open()
                }
            }
        }

        IrcCommand {
            id: ircCommand
        }

        ConnectionSheet {
            id: connectionSheet

            Component.onCompleted: {
                connectionSheet.host = Settings.host;
                connectionSheet.port = Settings.port;
                connectionSheet.name = Settings.name;
                connectionSheet.channel = Settings.channel;
                connectionSheet.secure = Settings.secure;
            }

            Component {
                id: sessionComponent
                Session {
                    id: session
                    property string channel
                    onConnected: {
                        if (channel.length) {
                            var cmd = ircCommand.createJoin(channel);
                            session.sendCommand(cmd);
                        }
                    }
                }
            }

            onAccepted: {
                var session = sessionComponent.createObject(window);
                session.userName = "communi";
                session.nickName = connectionSheet.name;
                session.realName = connectionSheet.name;
                session.host = connectionSheet.host;
                session.port = connectionSheet.port;
                session.channel = connectionSheet.channel;
                session.password = connectionSheet.password;
                session.secure = connectionSheet.secure;
                SessionManager.addSession(session);

                connectionSheet.password = "";
                Settings.host = connectionSheet.host;
                Settings.port = connectionSheet.port;
                Settings.name = connectionSheet.name;
                Settings.channel = connectionSheet.channel;
                Settings.secure = connectionSheet.secure;
            }
        }

        ChatSheet {
            id: channelSheet
            value: "#"
            titleText: qsTr("Join channel")
            description: qsTr("Channel")
            onAccepted: {
                var item = SessionModel[channelSheet.session];
                if (item) {
                    var cmd = ircCommand.createJoin(channelSheet.value);
                    item.session.sendCommand(cmd);
                }
            }
        }

        ChatSheet {
            id: querySheet
            titleText: qsTr("Open query")
            description: qsTr("User")
            onAccepted: {
                var item = SessionModel[querySheet.session];
                if (item)
                    item.addChild(querySheet.value);
            }
        }
    }
}
