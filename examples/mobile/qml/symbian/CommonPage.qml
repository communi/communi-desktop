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

    property alias header: header.data
    property alias __headerItem: header
    default property alias content: content.data

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
        }
        height: parent.height - header.height + (tools ? tools.height : 0) - (inputContext.visible ? inputContext.height : 0)
    }

    Item {
        id: header
        width: parent.width
        height: childrenRect.height
    }
}
