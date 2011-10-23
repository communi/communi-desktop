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

    property IrcSession session: null
    property QtObject modelData: null

    function sendMessage(receiver, message) {
        var cmd = CommandParser.parseCommand(receiver, message);
        if (cmd && modelData) {
            session.sendCommand(cmd);
            if (cmd.type == IrcCommand.Message || cmd.type == IrcCommand.CtcpAction)
            {
                var msg = ircMessage.fromCommand(session.nickName, cmd);
                modelData.receiveMessage(msg);
                msg.destroy();
            }
        }
    }

    title: modelData ? modelData.title : ""
    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: root.pageStack.pop()
        }
        ToolIcon {
            iconId: "toolbar-new-message"
            onClicked: {
                textField.visible = true;
                textField.forceActiveFocus();
            }
        }
    }

    // TODO: how to make it possible to access both Message.Type and
    //       Message.fromCommand() without creating a dummy instance?
    IrcMessage {
        id: ircMessage
    }

    onModelDataChanged: {
        listView.currentIndex = -1;
        if (modelData) {
            session = modelData.session;
            listView.model = modelData.messages;
            listView.currentIndex = listView.count - modelData.unseen - 1;
            Completer.modelItem = modelData;
        }
    }

    Connections {
        target: modelData
        onRemoved: page.pageStack.pop()
    }
    onBannerClicked: pageStack.pop()

    ListView {
        id: listView

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

        onHeightChanged: listView.positionViewAtEnd()
        onCountChanged: {
            if (!moving) listView.positionViewAtEnd();
            if (currentIndex == -1) currentIndex = count - 2;
        }

        highlight: Item {
            y: listView.currentItem !== null ? listView.currentItem.y : 0
            visible: listView.currentItem !== null && listView.currentIndex < listView.count - 1
            Rectangle {
                width: listView.width
                height: 1
                color: "red"
                anchors.bottom: parent.bottom
            }
        }
        highlightFollowsCurrentItem: true
    }

    ScrollDecorator {
        flickableItem: listView
        anchors.rightMargin: -UI.PAGE_MARGIN
    }

    Timer {
        id: timer
        interval: 50
        onTriggered: listView.positionViewAtEnd()
    }

    TextField {
        id: textField
        height: 0
        visible: false
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        platformSipAttributes: SipAttributes { actionKeyHighlighted: true }

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        onActiveFocusChanged: {
            textField.height = activeFocus ? textField.implicitHeight : 0;
            if (!activeFocus) {
                textField.visible = false;
                inputContext.reset();
                textField.text = "";
            }
            timer.start();
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
