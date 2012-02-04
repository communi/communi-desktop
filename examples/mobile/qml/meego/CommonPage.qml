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

Page {
    id: page

    property alias title: label.text
    property alias header: header
    property alias busy: indicator.running
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

        width: parent.width
        height: screen.currentOrientation === Screen.Landscape ? 46 : 72
        source: "image://theme/meegotouch-sheet-header-background"

        border {
            top: 10
            left: 10
            right: 10
            bottom: 2
        }

        MouseArea {
            anchors.fill: parent
        }

        Row {
            spacing: UI.DEFAULT_SPACING
            anchors {
                fill: parent
                leftMargin: UI.PAGE_MARGIN
                rightMargin: UI.PAGE_MARGIN
            }

            Label {
                id: label

                elide: Text.ElideRight
                font.pixelSize: UI.FONT_LARGE
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - indicator.width - UI.DEFAULT_SPACING
            }

            BusyIndicator {
                id: indicator
                visible: running
                anchors.verticalCenter: parent.verticalCenter
                MouseArea {
                    anchors.fill: parent
                    onClicked: indicator.busy = false
                }
            }

//            Image {
//                id: indicator
//                width: 32
//                height: 32
//                visible: SessionModel.length > 0
//                source: mouseArea.pressed ? "image://theme/icon-m-common-gray"
//                      : SessionManager.online ? "image://theme/icon-m-presence-online"
//                      : SessionManager.offline ? "image://theme/icon-m-presence-offline"
//                      : "image://theme/icon-m-presence-unknown"
//                anchors.verticalCenter: parent.verticalCenter

//                MouseArea {
//                    id: mouseArea
//                    width: parent.width * 2
//                    height: header.height
//                    anchors.centerIn: parent
//                    onClicked: {
//                        if (SessionManager.offline)
//                            SessionManager.ensureNetwork();
//                    }
//                }
//            }
        }
    }
}
