import QtQuick 1.0
import QtDesktop 0.1

Tab {
    id: tab

    signal connect(string host, string name)

    function receiveMessage(message) {
        if (textEdit.text.length > 0)
            textEdit.text += "\n";
        textEdit.text += MessageFormatter.formatMessage(message);
        scrollArea.verticalScrollBar.value = scrollArea.verticalScrollBar.maximumValue;
    }

    ScrollArea {
        id: scrollArea
        visible: false
        anchors.fill: parent
        contentHeight: textEdit.height
        horizontalScrollBar.visible: false
        TextEdit {
            id: textEdit
            readOnly: true
            width: scrollArea.viewportWidth
            wrapMode: Text.Wrap
        }
    }

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

        Button {
            id: button
            text: qsTr("Connect")
            enabled: hostField.text != "" && nameField.text != ""
            anchors.right: parent.right
            onClicked: {
                tab.connect(hostField.text, nameField.text);
                column.visible = false;
                scrollArea.visible = true;
            }
        }
    }
}
