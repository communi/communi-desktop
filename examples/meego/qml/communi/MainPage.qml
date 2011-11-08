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

    property alias bouncer: bouncer

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

    Component {
        id: bannerComponent
        InfoBanner {
            id: banner
            timerShowTime: 5000
            property QtObject item
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    bouncer.bounce(item);
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

    Connections {
        target: SessionManager
        onAlert: {
            var banner = bannerComponent.createObject(pageStack.currentPage.header);
            banner.text = item.alertText;
            banner.item = item;
            banner.show();
        }
    }

    ChatPage {
        id: chatPage
        function push(data) {
            modelData = data;
            root.pageStack.push(chatPage);
        }
        onStatusChanged: {
            if (modelData)
                modelData.current = (status !== PageStatus.Inactive);
            if (status == PageStatus.Inactive)
                modelData = null;
            if (status == PageStatus.Inactive && bouncer.item)
                bouncer.start();
        }
    }

    Timer {
        id: bouncer
        interval: 50
        property QtObject item
        function bounce(item) {
            if (root.status === PageStatus.Active) {
                chatPage.push(item);
            } else {
                bouncer.item = item;
                pageStack.pop();
            }
        }
        onTriggered: {
            chatPage.push(bouncer.item)
            bouncer.item = null;
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
