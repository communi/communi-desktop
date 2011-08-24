import QtQuick 1.0
import QtDesktop 0.1

Tab {
    id: tab

    Column {
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
            text: qsTr("Connect")
            enabled: hostField.text != "" && nameField.text != ""
            anchors.right: parent.right
            onClicked: {
                session.host = hostField.text;
                session.userName = nameField.text;
                session.nickName = nameField.text;
                session.realName = nameField.text;
                session.open();
                enabled = false;
            }
        }
    }
}
