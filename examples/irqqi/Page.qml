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
import Communi 1.0
import Communi.examples 1.0

Tab {
    id: tab

    function sendMessage(receiver, message) {
        var cmd = CommandParser.parseCommand(receiver, message);
        if (cmd) {
            session.sendCommand(cmd);
            if (cmd.type == IrcCommand.Message || cmd.type == IrcCommand.CtcpAction)
            {
                var msg = ircMessage.fromCommand(session.nickName, cmd);
                receiveMessage(msg);
                msg.destroy();
            }
        }
    }

    function receiveMessage(message) {
        textArea.text += formatter.formatMessage(message) + "<br/>";
        textArea.verticalValue = textArea.contentHeight;
    }

    function addUser(user) {
        // TODO
    }

    function removeUser(user) {
        // TODO
    }

    MessageFormatter {
        id: formatter
        timeStamp: true
        classFormat: false // workaround extra line breaks
    }

    Column {
        anchors.fill: parent
        TextArea {
            id: textArea
            readOnly: true
            wrapMode: Text.Wrap
            width: parent.width
            height: parent.height - textField.height
            horizontalScrollBar.visible: false
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
