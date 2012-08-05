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

Page {
    id: page

    property alias title: title.text
    property alias subtitle: subtitle.text
    property alias header: header
    property bool busy: false
    property bool active: true
    default property alias content: content.data

    Connections {
        target: Qt.application
        onActiveChanged: {
            if (!Qt.application.active)
                page.busy = false;
        }
    }

    Rectangle {
        id: background

        z: -1
        clip: true
        anchors.fill: parent
        color: platformStyle.colorBackgroundInverted

        Image {
            source: "../images/background.png"
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }

    Item {
        id: content

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    BorderImage {
        id: header

        property bool expanded: false
        property real baseHeight: implicitHeight //screen.currentOrientation === Screen.Landscape ? UI.HTB_LANDSCAPE_HEIGHT : UI.HTB_PORTRAIT_HEIGHT

        width: parent.width
        height: baseHeight + (expanded && subtitle ? subtitle.height + UI.DEFAULT_SPACING : 0)
        source: privateStyle.imagePath("qtg_fr_toolbar", true)

        Behavior on height { NumberAnimation { duration: 100 } }

        border {
            top: 20
            left: 20
            right: 20
            bottom: 20
        }

        MouseArea {
            id: mouseArea
            clip: true
            anchors.fill: parent
            onClicked: {
                page.busy = false;
                header.expanded = !header.expanded;
            }

            ListItemText {
                id: title

                role: "Title"
                verticalAlignment: Text.AlignVCenter
                mode: page.active ? "normal" : "disabled"
                platformInverted: true

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: indicator.left
                anchors.leftMargin: UI.PAGE_MARGIN
                anchors.rightMargin: UI.DEFAULT_SPACING
                height: header.baseHeight
            }

            BusyIndicator {
                id: indicator

                running: page.busy
                visible: page.busy

                anchors.right: parent.right
                anchors.rightMargin: UI.PAGE_MARGIN
                anchors.verticalCenter: title.verticalCenter

                MouseArea {
                    anchors.fill: parent
                    onClicked: indicator.running = false
                }

                platformInverted: true
            }

            Image {
                source: privateStyle.imagePath("toolbar-next", true)
                rotation: header.expanded ? 270 : 90
                anchors.centerIn: indicator
                visible: page.subtitle && mouseArea.pressed && mouseArea.containsMouse
                width: 16; height: 16
            }

            ListItemText {
                id: subtitle

                role: "SubTitle"
                mode: page.active ? "normal" : "disabled"
                platformInverted: true
                wrapMode: Text.Wrap

                anchors.top: title.bottom
                anchors.left: title.left
                anchors.right: indicator.right

                onLinkActivated: {
                    page.busy = true;
                    Qt.openUrlExternally(link);
                }
            }
        }
    }
}
