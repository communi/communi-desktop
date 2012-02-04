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

    property alias channel: channelField.text
    property alias password: passwordField.text
    property bool passwordRequired: false
    property int sessionIndex: buttons.checkedButton ? buttons.checkedButton.idx : -1

    acceptable: !channelField.errorHighlight && !passwordField.errorHighlight
    onStatusChanged: {
        if (status == DialogStatus.Open) channelField.forceActiveFocus();
        else if (status == DialogStatus.Closed) passwordRequired = false;
    }

    Column {
        id: column
        width: parent.width
        spacing: UI.DEFAULT_SPACING

        Column {
            width: parent.width
            visible: SessionModel.length > 1
            Label { text: qsTr("Connection") }
            ButtonColumn {
                id: buttons
                width: parent.width
                Repeater {
                    model: SessionModel
                    Button {
                        property int idx: index
                        text: modelData.title + " ("+ modelData.subtitle +")"
                    }
                }
            }
        }

        Column {
            width: parent.width
            Label { text: qsTr("Channel") }
            TextField {
                id: channelField
                text: qsTr("#")
                enabled: !passwordRequired
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                errorHighlight: text.length < 2 || (text[0] !== "#" && text[0] !== "&")
                width: parent.width
                platformSipAttributes: SipAttributes {
                    actionKeyHighlighted: true
                    actionKeyLabel: qsTr("Next")
                }
                Keys.onReturnPressed: passwordField.forceActiveFocus()
            }
        }

        Column {
            width: parent.width
            Label { text: qsTr("Password") }
            TextField {
                id: passwordField
                echoMode: TextInput.PasswordEchoOnEdit
                placeholderText: sheet.passwordRequired ? qsTr("Required!") : qsTr("Optional...")
                errorHighlight: sheet.passwordRequired ? !text.length : false
                visible: placeholderText.length
                width: parent.width
                platformSipAttributes: SipAttributes {
                    actionKeyEnabled: sheet.acceptable
                    actionKeyHighlighted: true
                    actionKeyLabel: qsTr("Ok")
                }
                Keys.onReturnPressed: sheet.accept()
            }
        }
    }
}
