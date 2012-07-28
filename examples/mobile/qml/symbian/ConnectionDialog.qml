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

    property alias host: hostField.text
    property alias port: portField.text
    property alias password: passField.text
    property alias name: nameField.text
    property alias user: userField.text
    property alias real: realField.text
    property alias channel: chanField.text
    property alias secure: secureBox.checked

    titleText: qsTr("Add connection")

    onStatusChanged: if (status == DialogStatus.Open) hostField.forceActiveFocus()

    Column {
        width: parent.width
        Column {
            width: parent.width
            spacing: UI.DEFAULT_SPACING

            Row {
                width: parent.width
                spacing: UI.DEFAULT_SPACING
                Column {
                    width: parent.width * 3/5 - UI.DEFAULT_SPACING/2
                    Label { text: qsTr("Host"); platformInverted: true }
                    TextField {
                        id: hostField
                        text: "irc.freenode.net"
                        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhUrlCharactersOnly
                        width: parent.width
                        errorHighlight: !text.length
                        Keys.onReturnPressed: portField.forceActiveFocus()
                        platformInverted: true
                    }
                }
                Column {
                    width: parent.width * 2/5 - UI.DEFAULT_SPACING/2
                    Label { text: qsTr("Port"); platformInverted: true }
                    TextField {
                        id: portField
                        text: "6667"
                        inputMethodHints: Qt.ImhDigitsOnly
                        width: parent.width
                        errorHighlight: !text.length
                        Keys.onReturnPressed: passField.forceActiveFocus()
                        platformInverted: true
                    }
                }
            }

            Row {
                width: parent.width
                spacing: UI.DEFAULT_SPACING
                Column {
                    id: passColumn
                    width: hostField.width
                    Label { text: qsTr("Password"); platformInverted: true }
                    TextField {
                        id: passField
                        echoMode: TextInput.PasswordEchoOnEdit
                        width: parent.width
                        Keys.onReturnPressed: nameField.forceActiveFocus()
                        platformInverted: true
                    }
                }
                CheckBox {
                    id: secureBox
                    text: qsTr("SSL")
                    anchors.bottom: passColumn.bottom
                    width: portField.width
                    platformInverted: true
                }
            }

            Row {
                width: parent.width
                spacing: UI.DEFAULT_SPACING
                Column {
                    width: parent.width / 2 - UI.DEFAULT_SPACING/2
                    Label { text: qsTr("Nick name"); platformInverted: true }
                    TextField {
                        id: nameField
                        text: "Guest" + Math.floor(Math.random() * 12345)
                        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                        width: parent.width
                        errorHighlight: !text.length
                        Keys.onReturnPressed: chanField.forceActiveFocus()
                        platformInverted: true
                    }
                }
                Column {
                    width: parent.width / 2 - UI.DEFAULT_SPACING/2
                    Label { text: qsTr("Channel"); platformInverted: true }
                    TextField {
                        id: chanField
                        text: "#communi"
                        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                        width: parent.width
                        Keys.onReturnPressed: userField.forceActiveFocus()
                        platformInverted: true
                    }
                }
            }

            Row {
                width: parent.width
                spacing: UI.DEFAULT_SPACING
                Column {
                    width: parent.width / 2 - UI.DEFAULT_SPACING/2
                    Label { text: qsTr("User name"); platformInverted: true }
                    TextField {
                        id: userField
                        text: "communi"
                        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                        width: parent.width
                        errorHighlight: !text.length
                        Keys.onReturnPressed: realField.forceActiveFocus()
                        platformInverted: true
                    }
                }
                Column {
                    width: parent.width / 2 - UI.DEFAULT_SPACING/2
                    Label { text: qsTr("Real name"); platformInverted: true }
                    TextField {
                        id: realField
                        text: "Communi for Symbian user"
                        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                        width: parent.width
                        errorHighlight: !text.length
                        Keys.onReturnPressed: hostField.forceActiveFocus()
                        platformInverted: true
                    }
                }
            }
        }
    }
}
