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

import QtQuick 1.0
import QtDesktop 0.1

Dialog {
    id: dialog

    width: 1.5 * column.width
    height: 2.0 * column.height

    property alias host: hostField.text
    property alias name: nameField.text

    buttons: host.length && name.length ? (ok | cancel) : cancel

    Column {
        id: column
        anchors.centerIn: parent
        spacing: 6

        Grid {
            columns: 2
            spacing: 6

            Text { text: qsTr("Host:") }
            TextField {
                id: hostField
                text: "irc.freenode.net"
            }

            Text { text: qsTr("Name:") }
            TextField { id: nameField }
        }
    }
}
