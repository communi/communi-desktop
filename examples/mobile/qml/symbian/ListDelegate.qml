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
import com.nokia.symbian 1.1
import com.nokia.extras 1.1
import "UIConstants.js" as UI

ListItem {
    id: root

    platformInverted: true
    subItemIndicator: !loader.sourceComponent

    Image {
        id: icon
        source: modelData.icon + ".png"
        anchors.left: parent.left
        anchors.leftMargin: UI.PAGE_MARGIN
        anchors.verticalCenter: parent.verticalCenter
    }

    Column {
        anchors {
            left: icon.right
            right: loader.left
            leftMargin: UI.DEFAULT_SPACING
            rightMargin: UI.DEFAULT_SPACING
            verticalCenter: parent.verticalCenter
        }

        ListItemText {
            id: title
            role: "Title"
            mode: root.mode
            platformInverted: root.platformInverted
            color: modelData.highlighted ? "red" : platformStyle.colorNormalLightInverted
            width: parent.width
            text: modelData.title
        }

        ListItemText {
            id: subtitle
            role: "SubTitle"
            mode: root.mode
            platformInverted: root.platformInverted
            width: parent.width
            text: modelData.subtitle
        }
    }

    Loader {
        id: loader
        width: 32
        height: 32
        anchors.right: parent.right
        anchors.rightMargin: UI.PAGE_MARGIN
        anchors.verticalCenter: parent.verticalCenter

        sourceComponent: modelData.busy ? busyIndicator : modelData.unreadCount ? countBubble : null

        Component {
            id: busyIndicator
            BusyIndicator { running: modelData.busy }
        }

        Component {
            id: countBubble
            Image {
                source: "squircle.png"
                Label {
                    id: unread
                    color: "white"
                    text: modelData.unreadCount
                    anchors.fill: parent
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
