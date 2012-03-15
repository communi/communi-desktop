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
import com.nokia.extras 1.1
import "UIConstants.js" as UI

CommonPage {
    id: root

    property alias bouncer: bouncer

    title: qsTr("Communi")
    tools: ToolBarLayout {
        ToolButton {
            iconSource: "toolbar-back"
            onClicked: Qt.quit()
            platformInverted: true
        }
        ToolButton {
            iconSource: "toolbar-add"
            onClicked: connectionDialog.open()
            platformInverted: true
        }
    }

    ListView {
        id: listView

        property QtObject currentSessionItem
        property QtObject currentSession
        property QtObject currentChildItem

        anchors.fill: parent
        model: SessionModel
        delegate: Column {
            width: parent.width
            ListDelegate {
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
                onPressAndHold: {
                    channelDialog.session = modelData.session;
                    listView.currentSessionItem = modelData;
                    listView.currentSession = modelData.session;
                    if (modelData.session.active)
                        activeSessionMenu.open();
                    else
                        inactiveSessionMenu.open();
                }
            }
            Repeater {
                id: repeater
                model: modelData.childItems
                ListDelegate {
                    title: modelData.title
                    subtitle: modelData.subtitle
                    iconSource: modelData.channel ? "../images/channel.png" : "../images/query.png"
                    highlighted: modelData.highlighted
                    active: modelData.session.active
                    unreadCount: modelData.unreadCount
                    busy: modelData.busy
                    onClicked: chatPage.push(modelData)
                    onPressAndHold: {
                        listView.currentChildItem = modelData;
                        listView.currentSession = modelData.session;
                        childMenu.open();
                    }
                }
            }
            ListHeading {
                height: 2
                visible: index < listView.count - 1
            }
        }
    }

    ScrollDecorator {
        flickableItem: listView
        platformInverted: true
    }

    Component {
        id: bannerComponent
        InfoBanner {
            id: banner
            timeout: 5000
            property QtObject item
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    bouncer.bounce(item, null);
                    banner.close();
                }
            }
            Connections {
                target: root.pageStack
                onCurrentPageChanged: banner.close()
            }
            onVisibleChanged: {
                if (!banner.visible)
                    banner.destroy();
            }
            platformInverted: true
        }
    }

    property variant effect: null
    Component.onCompleted: {
        var component = Qt.createComponent("Feedback.qml");
        if (component.status === Component.Ready)
            effect = component.createObject(root);
    }

    Connections {
        target: SessionManager
        onAlert: {
            var banner = bannerComponent.createObject(pageStack.currentPage);
            banner.text = item.alertText;
            banner.item = item;
            banner.open();
            if (root.effect)
                root.effect.play();
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
                else if (status === PageStatus.Active && bouncer.cmd) {
                    modelData.session.sendUiCommand(bouncer.cmd);
                    bouncer.cmd = null;
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
        property QtObject cmd
        function bounce(item, cmd) {
            bouncer.cmd = cmd;
            if (root.status === PageStatus.Active) {
                bouncer.item = null;
                chatPage.push(item);
            } else {
                bouncer.item = item;
                pageStack.pop();
            }
        }
        onTriggered: {
            chatPage.push(bouncer.item);
            bouncer.item = null;
        }
    }

    ContextMenu {
        id: activeSessionMenu

        MenuLayout {
            MenuItem {
                text: qsTr("Join channel")
                onClicked: channelDialog.open()
                platformInverted: true
            }
            MenuItem {
                text: qsTr("Open query")
                onClicked: queryDialog.open()
                platformInverted: true
            }
            MenuItem {
                text: qsTr("Set nick")
                onClicked: {
                    nickDialog.name = listView.currentSession.nickName;
                    nickDialog.open();
                }
                platformInverted: true
            }
            MenuItem {
                text: qsTr("Disconnect")
                onClicked: {
                    listView.currentSession.quit(ApplicationName);
                }
                platformInverted: true
            }
        }
        platformInverted: true
    }

    ContextMenu {
        id: inactiveSessionMenu

        MenuLayout {
            MenuItem {
                text: qsTr("Reconnect")
                onClicked: {
                    listView.currentSession.reconnect();
                }
                platformInverted: true
            }
            MenuItem {
                text: qsTr("Close")
                onClicked: {
                    SessionManager.removeSession(listView.currentSession);
                    listView.currentSession.destructLater();
                }
                platformInverted: true
            }
        }
        platformInverted: true
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
            var session = sessionComponent.createObject(root);
            session.nickName = connectionDialog.name;
            session.userName = connectionDialog.name;
            session.realName = connectionDialog.name;
            session.host = connectionDialog.host;
            session.port = connectionDialog.port;
            session.password = connectionDialog.password;
            session.secure = connectionDialog.secure;
            if (connectionDialog.channel.length)
                session.addChannel(connectionDialog.channel);

            SessionManager.addSession(session);
            session.reconnect();

            connectionDialog.password = "";
            Settings.host = connectionDialog.host;
            Settings.port = connectionDialog.port;
            Settings.name = connectionDialog.name;
            Settings.channel = connectionDialog.channel;
            Settings.secure = connectionDialog.secure;
        }
    }

    IrcCommand {
        id: ircCommand
    }

    ChannelDialog {
        id: channelDialog
        titleText: qsTr("Join channel")
        onAccepted: {
            var child = listView.currentSessionItem.addChild(channel);
            var cmd = ircCommand.createJoin(channel, password);
            listView.currentSession.sendUiCommand(cmd);
            bouncer.bounce(child, null);
        }
        Connections {
            target: SessionManager
            onChannelKeyRequired: {
                channelDialog.session = session;
                channelDialog.channel = channel;
                channelDialog.passwordRequired = true;
                channelDialog.open();
            }
        }
    }

    NameDialog {
        id: queryDialog
        titleText: qsTr("Open query")
        onAccepted: {
            var child = listView.currentSessionItem.addChild(name);
            var cmd = ircCommand.createWhois(name);
            bouncer.bounce(child, cmd);
        }
    }

    NameDialog {
        id: nickDialog
        titleText: qsTr("Set nick")
        onAccepted: {
            listView.currentSession.nickName = name;
        }
    }

    ContextMenu {
        id: childMenu

        MenuLayout {
            MenuItem {
                text: listView.currentChildItem !== null && listView.currentChildItem.channel && listView.currentSession.connected ? qsTr("Part") : qsTr("Close")
                onClicked: {
                    var item = listView.currentChildItem;
                    if (item.channel) {
                        var cmd = ircCommand.createPart(item.title, ApplicationName);
                        item.session.sendUiCommand(cmd);
                    }
                    item.sessionItem.removeChild(item.title);
                }
                platformInverted: true
            }
        }
        platformInverted: true
    }
}
