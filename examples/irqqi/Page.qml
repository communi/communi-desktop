import QtQuick 1.0
import QtDesktop 0.1

Tab {
    id: tab

    signal sendMessage(string receiver, string message)

    function receiveMessage(message) {
        textEdit.text += MessageFormatter.formatMessage(message);
    }

    Column {
        anchors.fill: parent
        ScrollArea {
            id: scrollArea
            width: parent.width
            height: parent.height - textField.height
            contentHeight: textEdit.height
            horizontalScrollBar.visible: false
            TextEdit {
                id: textEdit
                readOnly: true
                width: scrollArea.viewportWidth
                wrapMode: Text.Wrap
            }
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
