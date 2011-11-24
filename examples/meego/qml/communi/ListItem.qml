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

Item {
    id: root

    signal clicked
    signal pressAndHold

    width: listView.width
    height: 88

    BorderImage {
        id: background
        anchors.fill: parent
        visible: mouseArea.pressed
        source: "image://theme/meegotouch-list-background-pressed-center"
    }

    Image {
        id: icon
        source: "image://theme/" + modelData.icon
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

        Label {
            id: title
            font.bold: true
            font.pixelSize: 26
            font.family: "Nokia Pure Text"
            color: modelData.highlighted ? "red" : "#282828"
            width: parent.width
            elide: Text.ElideRight
            text: modelData.title
        }

        Label {
            id: subtitle
            font.pixelSize: 22
            font.family: "Nokia Pure Text Light"
            color: "#505050"
            width: parent.width
            elide: Text.ElideRight
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

        sourceComponent: modelData.busy ? busyIndicator : modelData.unreadCount ? countBubble : moreIndicator

        Component {
            id: busyIndicator
            BusyIndicator { running: modelData.busy }
        }

        Component {
            id: countBubble
            CountBubble { value: modelData.unreadCount; largeSized: true }
        }

        Component {
            id: moreIndicator
            MoreIndicator { }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.clicked()
        onPressAndHold: root.pressAndHold()
    }
}
