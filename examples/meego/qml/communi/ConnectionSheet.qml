import QtQuick 1.1
import com.nokia.meego 1.0
import "UIConstants.js" as UI

CommonSheet {
    id: sheet

    property alias host: hostField.text
    property alias port: portField.text
    property alias password: passField.text
    property alias name: nameField.text
    property alias channel: chanField.text

    acceptable: name != "" && host != ""
    titleText: qsTr("Add connection")

    Column {
        id: column
        anchors.fill: parent
        spacing: UI.DEFAULT_SPACING

        Row {
            width: parent.width
            spacing: UI.DEFAULT_SPACING
            Column {
                width: parent.width * 3/4 - UI.DEFAULT_SPACING/2
                Label { text: qsTr("Host") }
                TextField {
                    id: hostField
                    text: "irc.freenode.net"
                    width: parent.width
                }
            }
            Column {
                width: parent.width * 1/4 - UI.DEFAULT_SPACING/2
                Label { text: qsTr("Port") }
                TextField {
                    id: portField
                    text: "6667"
                    inputMethodHints: Qt.ImhDigitsOnly
                    width: parent.width
                }
            }
        }

        Row {
            width: parent.width
            spacing: UI.DEFAULT_SPACING
            Column {
                id: passColumn
                width: hostField.width
                Label { text: qsTr("Password") }
                TextField {
                    id: passField
                    echoMode: TextInput.PasswordEchoOnEdit
                    width: parent.width
                }
            }
            CheckBox {
                id: ssl
                enabled: false
                text: qsTr("SSL")
                anchors.bottom: passColumn.bottom
                width: portField.width
            }
        }

        Row {
            width: parent.width
            spacing: UI.DEFAULT_SPACING
            Column {
                width: parent.width / 2 - UI.DEFAULT_SPACING/2
                Label { text: qsTr("Name") }
                TextField {
                    id: nameField
                    width: parent.width
                    text: "Guest" + Math.floor(Math.random() * 12345)
                }
            }
            Column {
                width: parent.width / 2 - UI.DEFAULT_SPACING/2
                Label { text: qsTr("Channel") }
                TextField {
                    id: chanField
                    text: "#communi"
                    width: parent.width
                }
            }
        }
    }
}
