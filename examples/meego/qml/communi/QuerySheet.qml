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
import "UIConstants.js" as UI

CommonSheet {
    id: sheet

    property alias name: nameField.text
    property int sessionIndex: buttons.checkedButton ? buttons.checkedButton.idx : -1

    titleText: qsTr("Open query")
    acceptable: !nameField.errorHighlight
    onStatusChanged: if (status == DialogStatus.Open) nameField.forceActiveFocus()

    Column {
        id: column
        width: parent.width
        spacing: UI.DEFAULT_SPACING

        Label { text: qsTr("Connection"); visible: SessionModel.length > 1 }
        ButtonColumn {
            id: buttons
            width: parent.width
            visible: SessionModel.length > 1
            Repeater {
                model: SessionModel
                Button {
                    property int idx: index
                    text: modelData.title + " ("+ modelData.subtitle +")"
                }
            }
        }

        TextField {
            id: nameField
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
            placeholderText: qsTr("Name")
            errorHighlight: !text.length
            width: parent.width
            platformSipAttributes: SipAttributes {
                actionKeyEnabled: false
                actionKeyHighlighted: true
                actionKeyLabel: qsTr("Next")
            }
        }
    }
}
