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
import QtMultimediaKit 1.1
import QtMobility.feedback 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "UIConstants.js" as UI

CommonPage {
    id: root

    property alias bouncer: bouncer

    ListView {
        id: listView
        anchors.fill: parent
        model: SessionModel
        delegate: Column {
            width: parent.width
            ListItem {
                title: modelData.title
                subtitle: modelData.subtitle
                iconSource: modelData.error ? "../images/error.png" :
                           !modelData.session.connected &&
                           !modelData.session.active ? "../images/offline.png" :
                           "../images/server.png"
                highlighted: modelData.highlighted
                unreadCount: modelData.unreadCount
                busy: modelData.busy
                onClicked: chatPage.push(modelData)
                onPressAndHold: sessionMenu.popup(modelData.session)
            }
            Repeater {
                model: modelData.childItems
                ListItem {
                    title: modelData.title
                    subtitle: modelData.subtitle
                    iconSource: modelData.channel ? "../images/channel.png" : "../images/query.png"
                    highlighted: modelData.highlighted
                    unreadCount: modelData.unreadCount
                    busy: modelData.busy
                    onClicked: chatPage.push(modelData)
                    onPressAndHold: chatMenu.popup(modelData)
                }
            }
            Rectangle {
                color: "#b2b2b4"
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
        property QtObject session
        function popup(session) {
            sessionMenu.session = session;
            open();
        }
        MenuLayout {
            MenuItem {
                text: qsTr("Quit")
                onClicked: SessionManager.removeSession(sessionMenu.session)
            }
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
