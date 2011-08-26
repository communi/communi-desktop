import QtQuick 1.0
import QtDesktop 0.1

Window {
    id: dialog

    width: 1.5 * column.width
    height: 2.0 * column.height

    property alias host: hostField.text
    property alias name: nameField.text

    signal connect()

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
            enabled: dialog.host.length && dialog.name.length
            anchors.right: parent.right
            onClicked: dialog.connect()
        }
    }
}
