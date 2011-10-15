import QtQuick 1.1
import com.nokia.meego 1.0
import "UIConstants.js" as UI

CommonSheet {
    id: sheet

    property alias value: valueField.text
    property alias description: description.text
    property int session: buttons.checkedButton ? buttons.checkedButton.idx : -1

    acceptable: session != -1 && value != ""

    Column {
        id: column
        anchors.fill: parent
        spacing: UI.DEFAULT_SPACING

        Label { text: qsTr("Connection") }
        ButtonColumn {
            id: buttons
            width: parent.width
            Repeater {
                model: SessionModel
                Button {
                    property int idx: index
                    text: modelData.title + " ("+ modelData.subtitle +")"
                }
            }
        }

        Label { id: description }
        TextField {
            id: valueField
            width: parent.width
        }
    }
}
