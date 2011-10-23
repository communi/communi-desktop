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
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "UIConstants.js" as UI

CommonPage {
    id: root

    ListView {
        id: listView
        anchors.fill: parent
        model: SessionModel
        delegate: Column {
            width: parent.width
            ListItem {
                onClicked: chatPage.push(modelData)
                onPressAndHold: sessionMenu.popup(modelData.session)
            }
            Repeater {
                model: modelData.childItems
                ListItem {
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

    property QtObject bounceItem: null

    Connections {
        target: SessionManager
        onAlert: {
            var banner = root.banner;
            if (chatPage.status == PageStatus.Active)
                banner = chatPage.banner;
            bounceItem = item;
            banner.text = item.alertText;
            banner.show();
        }
    }

    onBannerClicked: chatPage.push(bounceItem)

    ChatPage {
        id: chatPage
        function push(data) {
            modelData = data;
            root.pageStack.push(chatPage);
            root.bounceItem = null;
        }
        onStatusChanged: {
            if (modelData)
                modelData.current = (status == PageStatus.Active);
            if (status == PageStatus.Inactive) {
                modelData = null;
                if (bounceItem)
                    bounceTimer.running = true;
            }
        }
    }

    Timer {
        id: bounceTimer
        interval: 50
        onTriggered: {
            if (bounceItem)
                chatPage.push(bounceItem);
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
