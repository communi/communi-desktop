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
import "UIConstants.js" as UI

CommonDialog {
    id: dialog

    default property alias content: contentItem.data

    buttonTexts: [qsTr("Ok"), qsTr("Cancel")]
    platformInverted: true
    privateCloseIcon: true

    onButtonClicked: {
        if (index === 0)
            dialog.accept();
        else
            dialog.reject();
    }

    content: Rectangle {
        id: background
        width: parent.width
        height: flickable.contentHeight + 2 * UI.PAGE_MARGIN
        color: "#e0e1e2"

        Flickable {
            id: flickable
            anchors.fill: parent
            anchors.margins: UI.PAGE_MARGIN
            contentHeight: contentItem.height

            Item {
                id: contentItem
                width: flickable.width
                height: childrenRect.height
            }
        }
    }
}
