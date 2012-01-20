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

    property alias name: nameField.text
    property alias sessionIndex: selectionDialog.selectedIndex

    titleText: qsTr("Open query")

    onStatusChanged: if (status == DialogStatus.Open) nameField.forceActiveFocus()

    Column {
        id: column
        width: parent.width
        spacing: UI.DEFAULT_SPACING

        Column {
            width: parent.width
            visible: SessionModel.length > 1
            Label { text: qsTr("Connection"); platformInverted: true }
            SelectionListItem {
                property QtObject sessionItem: SessionModel[Math.max(0, selectionDialog.selectedIndex)]
                title: sessionItem ? (sessionItem.title + " ("+ sessionItem.subtitle +")") : ""
                width: parent.width
                platformInverted: true

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
            Label { text: qsTr("Name"); platformInverted: true }
            TextField {
                id: nameField
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                errorHighlight: !text.length
                width: parent.width
                platformInverted: true
            }
        }
    }
}
