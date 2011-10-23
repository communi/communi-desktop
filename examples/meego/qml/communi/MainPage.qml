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
            }
            Repeater {
                model: modelData.childItems
                ListItem {
                    onClicked: chatPage.push(modelData)
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

    Connections {
        target: SessionManager
        onAlert: {
            var banner = root.banner;
            if (chatPage.status == PageStatus.Active)
                banner = chatPage.banner;
            banner.text = info;
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
            modelData.current = (status == PageStatus.Active);
            if (status == PageStatus.Inactive) modelData = null;
        }
    }
}
