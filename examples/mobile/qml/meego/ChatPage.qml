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
            modelData.session.sendUiCommand(cmd);
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

    active: modelData !== null && modelData.session.active
    title: modelData ? modelData.title : ""
    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: root.pageStack.pop()
        }
        ToolIcon {
            anchors.verticalCenter: parent.verticalCenter
            opacity: enabled ? 1.0 : UI.DISABLED_OPACITY
            enabled: clearItem.enabled || infoItem.enabled
            visible: modelData !== null && !indicator.visible
            iconId: "toolbar-view-menu"
            onClicked: contextMenu.open()
        }
        BusyIndicator {
            id: indicator
            visible: false
            running: visible
            anchors.verticalCenter: parent.verticalCenter
        }
        ToolIcon {
            iconId: "toolbar-new-message"
            opacity: enabled ? 1.0 : UI.DISABLED_OPACITY
            enabled: modelData !== null && modelData.session.active
            onClicked: {
                textField.visible = true;
                textField.forceActiveFocus();
                textField.openSoftwareInputPanel();
            }
        }
    }

    Keys.onReturnPressed: {
        textField.visible = true;
        textField.forceActiveFocus();
        textField.openSoftwareInputPanel();
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
                name = modelData.session.unprefixedUser(name);
                var child = modelData.sessionItem.addChild(name);
                bouncer.bounce(child, null);
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
            bottom: textField.visible ? textField.top : parent.bottom
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

        onCountChanged: if (!positioner.running) positioner.start()
        onHeightChanged: if (!positioner.running) positioner.start()

        currentIndex: modelData ? modelData.unseenIndex : -1
        highlight: Item {
            visible: listView.currentIndex > 0 && listView.currentIndex < listView.count - 1
            Image {
                source: "../images/right-arrow.png"
                anchors.left: parent.left
                anchors.leftMargin: -UI.PAGE_MARGIN
                anchors.verticalCenter: parent.bottom
            }
            Image {
                source: "../images/left-arrow.png"
                anchors.right: parent.right
                anchors.rightMargin: -UI.PAGE_MARGIN
                anchors.verticalCenter: parent.bottom
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
            if (!activeFocus) textField.visible = false;
            if (!positioner.running) positioner.start();
        }

        Keys.onReturnPressed: {
            page.sendMessage(page.title, textField.text);
            textField.text = "";
        }

        Connections {
            target: Qt.application
            onActiveChanged: {
                if (Qt.application.active && textField.activeFocus) {
                    textField.visible = true;
                    textField.openSoftwareInputPanel();
                }
            }
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
                onClicked: {
                    inputContext.reset();
                    if (textField.text.length)
                        textField.text = "";
                    else
                        page.forceActiveFocus();
                }
            }
        }
    }

    ContextMenu {
        id: contextMenu
        MenuLayout {
            MenuItem {
                id: clearItem
                text: qsTr("Clear")
                enabled: modelData !== null && listView.count
                onClicked: modelData.clear();
            }
            MenuItem {
                id: infoItem
                property bool chat: modelData !== null && modelData.channel !== undefined
                text: chat && modelData.channel ? qsTr("Names") : chat ? qsTr("Whois") : qsTr("Info")
                enabled: modelData !== null && modelData.channel !== undefined && modelData.session.active
                onClicked: {
                    var cmd = modelData.channel ? ircCommand.createNames(modelData.title)
                                                : ircCommand.createWhois(modelData.title);
                    modelData.sendUiCommand(cmd);
                    indicator.visible = true;
                }
            }
        }
    }
}
