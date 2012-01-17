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

BaseDialog {
    id: dialog

    property alias channel: channelField.text
    property alias password: passwordField.text
    property bool passwordRequired: false
    property alias sessionIndex: selectionDialog.selectedIndex

    titleText: qsTr("Join channel")

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
            Label { text: qsTr("Connection"); platformInverted: true }
            SelectionListItem {
                property QtObject sessionItem: SessionModel[Math.max(0, selectionDialog.selectedIndex)]
                title: sessionItem ? (sessionItem.title + " ("+ sessionItem.subtitle +")") : ""
                width: parent.width
                platformInverted: true
                visible: SessionModel.length > 1

                onClicked: selectionDialog.open()

                SelectionDialog {
                    id: selectionDialog
                    titleText: qsTr("Select connection")
                    platformInverted: true
                    model: SessionModel
                    delegate: MenuItem {
                        platformInverted: true
                        text: modelData.title + " ("+ modelData.subtitle +")"
                        onClicked: {
                            selectedIndex = index;
                            root.accept();
                        }
                    }
                }
            }
        }

        Column {
            width: parent.width
            Label { text: qsTr("Channel"); platformInverted: true }
            TextField {
                id: channelField
                text: qsTr("#")
                enabled: !passwordRequired
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                errorHighlight: text.length < 2 || (text[0] !== "#" && text[0] !== "&")
                width: parent.width
                Keys.onReturnPressed: passworld.forceActiveFocus()
                platformInverted: true
            }
        }

        Column {
            width: parent.width
            Label { text: qsTr("Password"); platformInverted: true }
            TextField {
                id: passwordField
                echoMode: TextInput.PasswordEchoOnEdit
                placeholderText: dialog.passwordRequired ? qsTr("Required") : qsTr("Optional")
                errorHighlight: dialog.passwordRequired ? !text.length : false
                visible: placeholderText.length
                width: parent.width
                Keys.onReturnPressed: channelField.forceActiveFocus()
                platformInverted: true
            }
        }
    }
}
