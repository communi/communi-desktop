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

    property QtObject modelData: null

    function sendMessage(receiver, message) {
        var cmd = CommandParser.parseCommand(receiver, message);
        if (cmd && modelData)
            modelData.sendCommand(cmd);
    }

    title: modelData ? modelData.title : ""
    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: root.pageStack.pop()
        }
        ToolIcon {
            anchors.verticalCenter: parent.verticalCenter
            visible: modelData !== null && modelData.channel !== undefined
            iconId: "toolbar-list"
            onClicked: {
                var cmd = modelData.channel ? ircCommand.createNames(modelData.title)
                                            : ircCommand.createWhois(modelData.title);
                modelData.sendCommand(cmd);
            }
        }
        ToolIcon {
            iconId: "toolbar-new-message"
            onClicked: {
                textField.visible = true;
                textField.forceActiveFocus();
            }
        }
    }

    onModelDataChanged: {
        //listView.currentIndex = -1;
        if (modelData) {
            listView.model = modelData.messages;
            //listView.currentIndex = listView.count - modelData.unseen - 1;
            Completer.modelItem = modelData;
        }
    }

    SelectionDialog {
        id: dialog
        property bool names: false
        function setContent(content) {
            dialog.model.clear();
            for (var i = 0; i < content.length; ++i)
                dialog.model.append({"name": content[i]});
        }
        titleText: modelData ? modelData.title : ""
        onAccepted: {
            if (names) {
                var name = model.get(selectedIndex).name;
                while (name.length && name[0] == "@" || name[0] == "+")
                    name = name.slice(1);
                bouncer.bounce(modelData.sessionItem.addChild(name));
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
            onLinkActivated: Qt.openUrlExternally(link)
        }

        onHeightChanged: if (!positioner.running) positioner.start()
        onCountChanged: {
            if (!positioner.running) positioner.start();
            //if (currentIndex == -1) currentIndex = count - 2;
        }

//        highlight: Item {
//            y: listView.currentItem !== null ? listView.currentItem.y : 0
//            visible: listView.currentItem !== null && listView.currentIndex < listView.count - 1
//            Rectangle {
//                width: listView.width
//                height: 1
//                color: "red"
//                anchors.bottom: parent.bottom
//            }
//        }
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
