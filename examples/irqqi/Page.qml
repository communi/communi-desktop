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

Tab {
    id: tab

    signal sendMessage(string receiver, string message)

    function receiveMessage(message) {
        textArea.text += MessageFormatter.formatMessage(message);
    }

    function addUser(user) {
        // TODO
    }

    function removeUser(user) {
        // TODO
    }

    Column {
        anchors.fill: parent
        TextArea {
            id: textArea
            readOnly: true
            wrapMode: Text.Wrap
            width: parent.width
            height: parent.height - textField.height
        }
        TextField {
            id: textField
            width: parent.width
            Keys.onReturnPressed: {
                tab.sendMessage(tab.title, textField.text);
                textField.text = "";
            }
        }
    }
}
