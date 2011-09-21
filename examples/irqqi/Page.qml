import QtQuick 1.0
import QtDesktop 0.1

Tab {
    id: tab

    signal sendMessage(string receiver, string message)

    function receiveMessage(message) {
        textArea.text += MessageFormatter.formatMessage(message);
    }

    function addUser(user) {
        // TODO
    }

    function removeUser(user) {
        // TODO
    }

    Column {
        anchors.fill: parent
        TextArea {
            id: textArea
            readOnly: true
            wrapMode: Text.Wrap
            width: parent.width
            height: parent.height - textField.height
        }
        TextField {
            id: textField
            width: parent.width
            Keys.onReturnPressed: {
                tab.sendMessage(tab.title, textField.text);
                textField.text = "";
            }
        }
    }
}
