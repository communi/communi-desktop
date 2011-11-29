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
import com.nokia.meego 1.0
import "UIConstants.js" as UI

CommonPage {
    id: page

    property alias count: listView.count
    property QtObject modelData: null

    function sendMessage(receiver, message) {
        var cmd = CommandParser.parseCommand(receiver, message);
        if (cmd && modelData) {
            modelData.session.sendCommand(cmd);
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
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: root.pageStack.pop()
        }
        ToolIcon {
            anchors.verticalCenter: parent.verticalCenter
            visible: modelData !== null && modelData.channel !== undefined && !indicator.visible
            iconId: "toolbar-list"
            onClicked: {
                var cmd = modelData.channel ? ircCommand.createNames(modelData.title)
                                            : ircCommand.createWhois(modelData.title);
                modelData.sendUiCommand(cmd);
                indicator.visible = true;
            }
        }
        BusyIndicator {
            id: indicator
            visible: false
            running: visible
            anchors.verticalCenter: parent.verticalCenter
        }
        ToolIcon {
            iconId: "toolbar-new-message"
            onClicked: {
                textField.visible = true;
                textField.forceActiveFocus();
            }
        }
    }

    Keys.onReturnPressed: {
        textField.visible = true;
        textField.forceActiveFocus();
    }

    onModelDataChanged: {
        if (modelData) {
            listView.model = modelData.messages;
            Completer.modelItem = modelData;
        }
        indicator.visible = false;
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
            indicator.visible = false;
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
            bottom: textField.top
            margins: UI.PAGE_MARGIN
        }

        delegate: Label {
            text: display
            width: listView.width
            wrapMode: Text.Wrap
            onLinkActivated: {
                page.busy = true;
                Qt.openUrlExternally(link);
            }
        }

        Connections {
            target: Qt.application
            onActiveChanged: {
                if (!Qt.application.active)
                    page.busy = false;
            }
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
    }

    Timer {
        id: positioner
        interval: 100
        onTriggered: if (!listView.moving) listView.positionViewAtEnd()
    }

    TextField {
        id: textField
        height: 0
        visible: false
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhUrlCharactersOnly
        platformSipAttributes: SipAttributes {
            actionKeyHighlighted: true
            actionKeyLabel: qsTr("Send")
            actionKeyEnabled: textField.text.length
        }

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
            page.sendMessage(page.title, textField.text);
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

        style: TextFieldStyle {
            paddingLeft: tabButton.width
            paddingRight: clearButton.width
        }

        Image {
            id: tabButton
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            source: "icon-m-input-tab.png"
            MouseArea {
                anchors.fill: parent
                onClicked: Completer.complete(textField.text, textField.selectionStart, textField.selectionEnd)
            }
        }

        Image {
            id: clearButton
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            source: "image://theme/icon-m-input-clear"
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    inputContext.reset();
                    textField.text = "";
                }
            }
        }
    }
}
