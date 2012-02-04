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

    property alias title: title.text
    property alias subtitle: subtitle.text
    property int lag: -1
    property alias iconSource: icon.source
    property bool highlighted: false
    property bool active: true
    property int unreadCount: 0
    property bool busy: false

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
        anchors.left: parent.left
        anchors.leftMargin: UI.PAGE_MARGIN
        anchors.verticalCenter: parent.verticalCenter

        Label {
            visible: root.lag >= 0
            text: {
                if (root.lag < 100) return root.lag + qsTr("ms");
                if (root.lag < 10000) return (root.lag / 1000).toFixed(1) + qsTr("s");
                return Math.ceil(root.lag / 1000) + qsTr("s");
            }
            anchors.centerIn: parent
            font.pixelSize: 18
            font.family: "Nokia Pure Text Light"
            font.bold: true
            color: "red"
        }
    }

    Column {
        id: column

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
            color: root.highlighted ? "red" : root.active ? "#282828" : "#b2b2b4"
            width: parent.width
            elide: Text.ElideRight
        }

        Label {
            id: subtitle
            font.pixelSize: 22
            font.family: "Nokia Pure Text"
            font.weight: Font.Light
            color: root.active ? "#505050" : "#b2b2b4"
            width: parent.width
            elide: Text.ElideRight
        }
    }

    Loader {
        id: loader

        anchors.right: parent.right
        anchors.rightMargin: UI.PAGE_MARGIN
        anchors.verticalCenter: parent.verticalCenter

        sourceComponent: root.busy ? busyIndicator : root.unreadCount ? countBubble : moreIndicator

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
                    font.pixelSize: 18
                    text: root.unreadCount
                    anchors.fill: parent
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
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
