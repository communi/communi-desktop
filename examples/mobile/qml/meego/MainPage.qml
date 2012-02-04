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
import QtMultimediaKit 1.1
import QtMobility.feedback 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "UIConstants.js" as UI

CommonPage {
    id: root

    property alias bouncer: bouncer

    title: qsTr("Communi")
    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-add"
            anchors.right: parent.right
            onClicked: connectionSheet.open()
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: SessionModel
        delegate: Column {
            width: parent.width
            ListItem {
                title: modelData.title
                subtitle: modelData.subtitle
                iconSource: modelData.session.currentLag > 10000 ? "../images/unknown.png" :
                            modelData.session.active && !modelData.session.connected ? "../images/server.png" :
                            modelData.session.connected ? "../images/connected.png" : "../images/disconnected.png"
                highlighted: modelData.highlighted
                active: modelData.session.active
                unreadCount: modelData.unreadCount
                busy: modelData.busy
                onClicked: chatPage.push(modelData)
                onPressAndHold: sessionMenu.popup(modelData)
                lag: modelData.session.currentLag
            }
            Repeater {
                model: modelData.childItems
                ListItem {
                    title: modelData.title
                    subtitle: modelData.subtitle
                    iconSource: modelData.channel ? "../images/channel.png" : "../images/query.png"
                    highlighted: modelData.highlighted
                    active: modelData.session.active
                    unreadCount: modelData.unreadCount
                    busy: modelData.busy
                    onClicked: chatPage.push(modelData)
                    onPressAndHold: chatMenu.popup(modelData)
                }
            }
            Rectangle {
                color: UI.SEPARATOR_COLOR
                height: 1
                visible: index < listView.count - 1
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: UI.PAGE_MARGIN
                    rightMargin: UI.PAGE_MARGIN
                }
            }
        }
    }

    ScrollDecorator {
        flickableItem: listView
    }

    Component {
        id: bannerComponent
        InfoBanner {
            id: banner
            timerShowTime: 5000
            property QtObject item
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    bouncer.bounce(item, null);
                    banner.hide();
                }
            }
            Connections {
                target: root.pageStack
                onCurrentPageChanged: banner.hide()
            }
            onVisibleChanged: {
                if (!banner.visible)
                    banner.destroy();
            }
        }
    }

    FileEffect {
        id: vibraEffect
        source: "file:///usr/share/sounds/vibra/tct_chat.ivt"
    }

    SoundEffect {
        id: soundEffect
        source: "file:///usr/share/sounds/ui-tones/snd_chat_fg.wav"
    }

    Connections {
        target: SessionManager
        onAlert: {
            var banner = bannerComponent.createObject(pageStack.currentPage.header);
            banner.text = item.alertText;
            banner.item = item;
            banner.show();
            if (Qt.application.active)
                soundEffect.play();
            else
                vibraEffect.start();
        }
    }

    ChatPage {
        id: chatPage
        function push(data) {
            modelData = data;
            root.pageStack.push(chatPage);
        }
        onStatusChanged: {
            if (modelData) {
                modelData.current = (chatPage.status === PageStatus.Active);
                if (status === PageStatus.Inactive) {
                    modelData.unseenIndex = chatPage.count - 1;
                    modelData = null;
                }
            }
            if (status === PageStatus.Inactive && bouncer.item)
                bouncer.start();
        }
    }

    Timer {
        id: bouncer
        interval: 50
        property QtObject item
        property QtObject cmd;
        function bounce(item, cmd) {
            if (root.status === PageStatus.Active) {
                chatPage.push(item);
                if (cmd !== null)
                    item.session.sendCommand(cmd);
            } else {
                bouncer.item = item;
                bouncer.cmd = cmd;
                pageStack.pop();
            }
        }
        onTriggered: {
            chatPage.push(bouncer.item)
            if (bouncer.cmd !== null)
                item.session.sendCommand(bouncer.cmd);
            bouncer.item = null;
            bouncer.cmd = null;
        }
    }

    ContextMenu {
        id: sessionMenu
        property QtObject sessionItem
        property bool active: sessionItem && sessionItem.session.active
        function popup(sessionItem) {
            sessionMenu.sessionItem = sessionItem;
            open();
        }
        MenuLayout {
            MenuItem {
                text: !sessionMenu.active ? qsTr("Connect") : qsTr("Disconnect")
                onClicked: {
                    if (!sessionMenu.active) {
                        if (sessionMenu.sessionItem.session.ensureNetwork())
                            sessionMenu.sessionItem.session.open();
                    } else {
                        sessionMenu.sessionItem.session.close();
                    }
                }
            }
            MenuItem {
                text: qsTr("Join channel")
                enabled: sessionMenu.sessionItem.session.connected
                onClicked: channelSheet.open()
            }
            MenuItem {
                text: qsTr("Open query")
                enabled: sessionMenu.sessionItem.session.connected
                onClicked: querySheet.open()
            }
            MenuItem {
                text: qsTr("Set nick")
                enabled: sessionMenu.sessionItem.session.connected
                onClicked: {
                    nickSheet.name = sessionMenu.sessionItem.session.nickName;
                    nickSheet.open();
                }
            }
            MenuItem {
                text: qsTr("Close")
                onClicked: SessionManager.removeSession(sessionMenu.session)
            }
        }
    }

    ConnectionSheet {
        id: connectionSheet
        title: qsTr("Add connection")

        Component.onCompleted: {
            connectionSheet.host = Settings.host;
            connectionSheet.port = Settings.port;
            connectionSheet.name = Settings.name;
            connectionSheet.user = Settings.user;
            connectionSheet.real = Settings.real;
            connectionSheet.channel = Settings.channel;
            connectionSheet.secure = Settings.secure;
        }

        Component {
            id: sessionComponent
            Session { }
        }

        onAccepted: {
            var session = sessionComponent.createObject(window);
            session.nickName = connectionSheet.name;
            session.userName = connectionSheet.user.length ? connectionSheet.user : "communi";
            session.realName = connectionSheet.real.length ? connectionSheet.real : connectionSheet.name;
            session.host = connectionSheet.host;
            session.port = connectionSheet.port;
            session.password = connectionSheet.password;
            session.secure = connectionSheet.secure;
            session.channels = connectionSheet.channel;
            SessionManager.addSession(session);

            connectionSheet.password = "";
            Settings.host = session.host;
            Settings.port = session.port;
            Settings.name = session.nickName;
            Settings.user = session.userName;
            Settings.real = session.realName;
            Settings.channel = connectionSheet.channel;
            Settings.secure = session.secure;
        }
    }

    IrcCommand {
        id: ircCommand
    }

    ChannelSheet {
        id: channelSheet
        title: qsTr("Join channel")
        onAccepted: {
            var child = sessionMenu.sessionItem.addChild(channel);
            var cmd = ircCommand.createJoin(channel, password);
            bouncer.bounce(child, cmd);
        }
        Connections {
            target: SessionManager
            onChannelKeyRequired: {
                channelSheet.channel = channel;
                channelSheet.passwordRequired = true;
                channelSheet.open();
            }
        }
    }

    NameSheet {
        id: querySheet
        title: qsTr("Open query")
        onAccepted: {
            var child = sessionMenu.sessionItem.addChild(name);
            var cmd = ircCommand.createWhois(name);
            bouncer.bounce(child, cmd);
        }
    }

    NameSheet {
        id: nickSheet
        title: qsTr("Set nick")
        onAccepted: {
            sessionMenu.sessionItem.session.nickName = name;
        }
    }

    ContextMenu {
        id: chatMenu
        property QtObject chatItem
        function popup(item) {
            chatItem = item;
            open();
        }
        MenuLayout {
            MenuItem {
                text: qsTr("Close")
                onClicked: chatMenu.chatItem.close()
            }
        }
    }
}
