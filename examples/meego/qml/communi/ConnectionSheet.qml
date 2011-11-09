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

    property alias host: hostField.text
    property alias port: portField.text
    property alias password: passField.text
    property alias name: nameField.text
    property alias channel: chanField.text
    property alias secure: secureBox.checked

    acceptable: name != "" && host != "" && port != ""
    titleText: qsTr("Add connection")

    onStatusChanged: if (status == DialogStatus.Open) hostField.forceActiveFocus()

    Column {
        id: column
        width: parent.width
        spacing: UI.DEFAULT_SPACING

        Row {
            width: parent.width
            spacing: UI.DEFAULT_SPACING
            Column {
                width: parent.width * 3/4 - UI.DEFAULT_SPACING/2
                Label { text: qsTr("Host") }
                TextField {
                    id: hostField
                    text: "irc.freenode.net"
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhUrlCharactersOnly
                    width: parent.width
                    errorHighlight: !text.length
                }
            }
            Column {
                width: parent.width * 1/4 - UI.DEFAULT_SPACING/2
                Label { text: qsTr("Port") }
                TextField {
                    id: portField
                    text: "6667"
                    inputMethodHints: Qt.ImhDigitsOnly
                    width: parent.width
                    errorHighlight: !text.length
                }
            }
        }

        Row {
            width: parent.width
            spacing: UI.DEFAULT_SPACING
            Column {
                id: passColumn
                width: hostField.width
                Label { text: qsTr("Password") }
                TextField {
                    id: passField
                    echoMode: TextInput.PasswordEchoOnEdit
                    width: parent.width
                }
            }
            CheckBox {
                id: secureBox
                text: qsTr("SSL")
                anchors.bottom: passColumn.bottom
                width: portField.width
            }
        }

        Row {
            width: parent.width
            spacing: UI.DEFAULT_SPACING
            Column {
                width: parent.width / 2 - UI.DEFAULT_SPACING/2
                Label { text: qsTr("Name") }
                TextField {
                    id: nameField
                    text: "Guest" + Math.floor(Math.random() * 12345)
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                    width: parent.width
                    errorHighlight: !text.length
                }
            }
            Column {
                width: parent.width / 2 - UI.DEFAULT_SPACING/2
                Label { text: qsTr("Channel") }
                TextField {
                    id: chanField
                    text: "#communi"
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                    width: parent.width
                }
            }
        }
    }
}
