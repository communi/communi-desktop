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
import "UIConstants.js" as UI

BaseDialog {
    id: dialog

    property alias timeStamp: timeStampBox.checked
    property alias stripNicks: stripNicksBox.checked

    titleText: qsTr("Settings")

    Column {
        width: parent.width
        Column {
            width: parent.width
            spacing: UI.DEFAULT_SPACING

            CheckBox {
                id: timeStampBox
                text: qsTr("Enable timestamps")
                platformInverted: true
            }
            CheckBox {
                id: stripNicksBox
                text: qsTr("Strip nicks")
                platformInverted: true
            }
        }
    }
}
