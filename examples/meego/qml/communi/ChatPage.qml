import QtQuick 1.1
import Communi 1.0
import com.nokia.meego 1.0
import "UIConstants.js" as UI

CommonPage {
    id: page

    property IrcSession session: null
    property alias model: listView.model
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

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: pageStack.pop()
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
        }

        onHeightChanged: listView.positionViewAtEnd()
        onCountChanged: if (!moving) listView.positionViewAtEnd()
    }

    Timer {
        id: timer
        interval: 50
        onTriggered: listView.positionViewAtEnd()
    }

    TextField {
        id: textField
        height: 0
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: UI.PAGE_MARGIN
        }

        onActiveFocusChanged: {
            textField.height = activeFocus ? textField.implicitHeight : 0;
            if (!activeFocus) {
                textField.visible = false;
                textField.text = "";
            }
            timer.start();
        }

        Keys.onReturnPressed: {
            if (model) {
                page.sendMessage(page.title, textField.text);
                parent.forceActiveFocus();
                textField.text = "";
            }
        }
    }
}
