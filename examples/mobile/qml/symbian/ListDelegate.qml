/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
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

    property alias title: title.text
    property alias subtitle: subtitle.text
    property alias iconSource: icon.source
    property bool highlighted: false
    property bool active: true
    property int unreadCount: 0
    property bool busy: false

    platformInverted: true
    subItemIndicator: !loader.sourceComponent

    Image {
        id: icon
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
            mode: root.active ? root.mode : "disabled"
            platformInverted: root.platformInverted
            color: root.highlighted ? "#ff3333" : platformStyle.colorNormalLightInverted
            width: parent.width
        }

        ListItemText {
            id: subtitle
            role: "SubTitle"
            mode: root.active ? root.mode : "disabled"
            platformInverted: root.platformInverted
            width: parent.width
        }
    }

    Loader {
        id: loader
        anchors.right: parent.right
        anchors.rightMargin: UI.PAGE_MARGIN
        anchors.verticalCenter: parent.verticalCenter

        sourceComponent: root.busy ? busyIndicator : root.unreadCount ? countBubble : null

        Component {
            id: busyIndicator
            BusyIndicator { running: root.busy }
        }

        Component {
            id: countBubble
            Image {
                source: "../images/squircle.png"
                Label {
                    id: unread
                    color: "white"
                    text: root.unreadCount
                    anchors.fill: parent
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
