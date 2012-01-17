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

Page {
    id: page

    property alias busy: indicator.running
    property alias title: title.text
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

        anchors.fill: parent
        color: "#e0e1e2"

        Image {
            source: "../images/background.png"
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }

    Column {
        anchors.fill: parent
        ToolBar {
            id: header
            Row {
                spacing: UI.DEFAULT_SPACING
                anchors {
                    fill: parent
                    leftMargin: UI.PAGE_MARGIN
                    rightMargin: UI.PAGE_MARGIN
                }

                ListItemText {
                    id: title
                    role: "Title"
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - indicator.width - UI.DEFAULT_SPACING
                    platformInverted: true
                }

                BusyIndicator {
                    id: indicator
                    visible: running
                    anchors.verticalCenter: parent.verticalCenter
                    MouseArea {
                        anchors.fill: parent
                        onClicked: indicator.running = false
                    }
                    platformInverted: true
                }
            }
            platformInverted: true
        }

        Item {
            id: content
            clip: true
            width: parent.width
            height: inputContext.visible ? parent.height - inputContext.height : parent.height - header.height
        }
    }
}
