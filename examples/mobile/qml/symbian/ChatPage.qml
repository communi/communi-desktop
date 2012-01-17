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
import Communi 1.0
import com.nokia.symbian 1.1
import "UIConstants.js" as UI

CommonPage {
    id: page

    property alias count: listView.count
    property QtObject modelData: null

    function sendMessage(receiver, message) {
        var cmd = CommandParser.parseCommand(receiver, message);
        if (cmd && modelData) {
            var ret = modelData.session.sendCommand(cmd);
            if (cmd.type == IrcCommand.Message || cmd.type == IrcCommand.CtcpAction) {
                var msg = ircMessage.fromCommand(modelData.session.nickName, cmd);
                modelData.receiveMessage(msg);
                msg.destroy();
            }
        }
    }

    // TODO: how to make it possible to access both Message.Type and
    //       Message.fromCommand() without creating a dummy instance?
    IrcMessage {
        id: ircMessage
    }

    title: modelData ? modelData.title : ""

    tools: ToolBarLayout {
        ToolButton {
            iconSource: "toolbar-back"
            onClicked: root.pageStack.pop()
            platformInverted: true
        }
        ToolButton {
            anchors.verticalCenter: parent.verticalCenter
            visible: modelData !== null && modelData.channel !== undefined // && !indicator.visible
            iconSource: "toolbar-list"
            onClicked: {
                var cmd = modelData.channel ? ircCommand.createNames(modelData.title)
                                            : ircCommand.createWhois(modelData.title);
                modelData.sendUiCommand(cmd);
//                indicator.visible = true;
            }
            platformInverted: true
        }
//        BusyIndicator {
//            id: indicator
//            visible: false
//            running: visible
//            anchors.verticalCenter: parent.verticalCenter
//        }
        ToolButton {
            iconSource: "toolbar-add"
            onClicked: {
                textField.visible = true;
                textField.openSoftwareInputPanel();
                textField.forceActiveFocus();
            }
            platformInverted: true
        }
    }

    Keys.onReturnPressed: {
        textField.visible = true;
        textField.openSoftwareInputPanel();
        textField.forceActiveFocus();
    }

    onModelDataChanged: {
        if (modelData) {
            listView.model = modelData.messages;
            Completer.modelItem = modelData;
        }
        //indicator.visible = false;
    }

    SelectionDialog {
        id: dialog
        model: ListModel { }
        property bool names: false
        function setContent(content) {
            dialog.model.clear();
            dialog.selectedIndex = -1;
            for (var i = 0; i < content.length; ++i)
                dialog.model.append({"name": content[i]});
            //indicator.visible = false;
        }
        titleText: modelData ? modelData.title : ""
        onAccepted: {
            if (names) {
                var name = model.get(selectedIndex).name;
                while (name.length && name[0] == "@" || name[0] == "+")
                    name = name.slice(1);

                var child = modelData.sessionItem.addChild(name);
                var cmd = ircCommand.createWhois(name);
                bouncer.bounce(child, cmd);
            }
        }
        platformInverted: true
    }

    Connections {
        target: modelData
        onRemoved: page.pageStack.pop()
        onNamesReceived: {
            dialog.setContent(names);
            dialog.names = true;
            dialog.open();
        }
        onWhoisReceived: {
            dialog.setContent(whois);
            dialog.names = false;
            dialog.open();
        }
    }

    ListView {
        id: listView

        cacheBuffer: listView.height * 2

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: textField.visible ? textField.top : parent.bottom
            margins: UI.PAGE_MARGIN
        }

        delegate: Label {
            text: display
            width: listView.width
            wrapMode: Text.Wrap
            onLinkActivated: {
                page.busy = true;
                //Qt.openUrlExternally(link);
            }
            platformInverted: true
        }

        onCountChanged: if (!positioner.running) positioner.start()
        onHeightChanged: if (!positioner.running) positioner.start()

        currentIndex: modelData ? modelData.unseenIndex : -1
        highlight: Item {
            visible: listView.currentIndex > 0 && listView.currentIndex < listView.count - 1
            Rectangle {
                width: listView.width
                height: 1
                color: "red"
                anchors.bottom: parent.bottom
            }
        }
    }

    ScrollDecorator {
        flickableItem: listView
        anchors.rightMargin: -UI.PAGE_MARGIN
        platformInverted: true
    }

    Timer {
        id: positioner
        interval: 100
        onTriggered: if (!listView.moving) listView.positionViewAtEnd()
    }

    MouseArea {
        enabled: textField.visible
        anchors.fill: parent
        onClicked: textField.visible = false
    }

    TextField {
        id: textField
        height: 0
        visible: false
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhUrlCharactersOnly
        platformInverted: true

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: UI.PAGE_MARGIN
        }

        onActiveFocusChanged: {
            textField.height = activeFocus ? textField.implicitHeight : 0;
            if (!activeFocus)
                textField.visible = false;
            if (!positioner.running) positioner.start();
        }

        Keys.onReturnPressed: {
            if (modelData)
                page.sendMessage(modelData.title, textField.text);
            parent.forceActiveFocus();
            textField.text = "";
        }

        Connections {
            target: Completer
            onCompleted: {
                textField.text = text;
                textField.select(selStart, selEnd);
            }
        }

        platformLeftMargin: tabButton.width
        platformRightMargin: clearButton.width

        Image {
            id: tabButton
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            source: "../images/tab.png"
            MouseArea {
                anchors.fill: parent
                onClicked: Completer.complete(textField.text, textField.selectionStart, textField.selectionEnd)
            }
        }

        Image {
            id: clearButton
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            source: "../images/clear.png"
            MouseArea {
                anchors.fill: parent
                onClicked: textField.text = ""
            }
        }
    }
}
