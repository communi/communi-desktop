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
import com.nokia.symbian 1.1
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
    subtitle: modelData ? modelData.description : ""
    tools: ToolBarLayout {
        ToolButton {
            id: backButton
            iconSource: "toolbar-back"
            onClicked: root.pageStack.pop()
            platformInverted: true
        }
        Loader {
            id: lister
            property bool busy: false
            width: backButton.width
            height: backButton.height
            anchors.verticalCenter: parent.verticalCenter
            sourceComponent: busy ? indicatorComponent : menuComponent
            Component {
                id: menuComponent
                ToolButton {
                    visible: modelData !== null
                    opacity: enabled ? 1.0 : UI.DISABLED_OPACITY
                    enabled: clearItem.active || infoItem.active
                    iconSource: "toolbar-menu"
                    onClicked: contextMenu.open()
                    platformInverted: true
                }
            }
            Component {
                id: indicatorComponent
                Item {
                    BusyIndicator {
                        running: true
                        anchors.centerIn: parent
                    }
                }
            }
        }

        ToolButton {
            iconSource: "toolbar-add"
            opacity: enabled ? 1.0 : UI.DISABLED_OPACITY
            enabled: modelData !== null && modelData.session.active
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
        lister.busy = false;
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
            lister.busy = false;
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
        platformInverted: true
    }

    Connections {
        target: modelData
        ignoreUnknownSignals: true
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
            platformInverted: true
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

    ScrollBar {
        id: scrollBar
        height: listView.height
        anchors { top: listView.top; right: listView.right; rightMargin: -UI.PAGE_MARGIN }
        flickableItem: listView
        interactive: false
        orientation: Qt.Vertical
        platformInverted: true
    }

    Timer {
        id: positioner
        interval: 100
        onTriggered: if (!listView.moving) listView.positionViewAtEnd()
    }

    Rectangle {
        visible: textField.visible
        anchors.fill: textField
        anchors.leftMargin: -UI.PAGE_MARGIN
        anchors.rightMargin: -UI.PAGE_MARGIN
        anchors.bottomMargin: -UI.PAGE_MARGIN
        color: platformStyle.colorBackgroundInverted
    }

    TextField {
        id: textField
        visible: false
        height: visible ? implicitHeight : 0
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhUrlCharactersOnly
        platformInverted: true

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: UI.PAGE_MARGIN
        }

        function send() {
            if (modelData)
                page.sendMessage(modelData.title, text);
            closeSoftwareInputPanel();
            text = "";
        }

        onVisibleChanged: if (!positioner.running) positioner.start()

        Keys.onEnterPressed: send()
        Keys.onReturnPressed: send()

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
                anchors.leftMargin: -UI.PAGE_MARGIN
                anchors.bottomMargin: -UI.PAGE_MARGIN
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
                anchors.rightMargin: -UI.PAGE_MARGIN
                anchors.bottomMargin: -UI.PAGE_MARGIN
                onClicked: {
                    textField.visible = false;
                    textField.closeSoftwareInputPanel();
                    textField.text = "";
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
                property bool active: modelData !== null && listView.count
                enabled: active
                onClicked: modelData.clear();
                platformInverted: true
            }
            MenuItem {
                id: infoItem
                property bool chat: modelData !== null && modelData.channel !== undefined
                text: chat && modelData.channel ? qsTr("Names") : chat ? qsTr("Whois") : qsTr("Info")
                property bool active: modelData !== null && modelData.channel !== undefined && modelData.session.active
                enabled: active
                onClicked: {
                    var cmd = modelData.channel ? ircCommand.createNames(modelData.title)
                                                : ircCommand.createWhois(modelData.title);
                    modelData.sendUiCommand(cmd);
                    lister.busy = true;
                }
                platformInverted: true
            }
        }
        platformInverted: true
    }
}
