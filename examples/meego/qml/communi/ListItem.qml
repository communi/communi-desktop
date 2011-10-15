import QtQuick 1.1
import com.nokia.meego 1.0
import "UIConstants.js" as UI

Item {
    id: root

    property alias title: title.text
    property alias subtitle: subtitle.text
    property bool highlighted: false

    signal clicked

    width: listView.width
    height: 88

    BorderImage {
        id: background
        anchors.fill: parent
        visible: mouseArea.pressed
        source: "image://theme/meegotouch-list-background-pressed-center"
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: UI.PAGE_MARGIN
            rightMargin: UI.PAGE_MARGIN
            verticalCenter: parent.verticalCenter
        }

        Label {
            id: title
            font.bold: true
            font.pixelSize: 26
            font.family: "Nokia Pure Text"
            color: highlighted ? "red" : "#282828"
            visible: text != ""
            width: root.width - UI.DEFAULT_SPACING - icon.width
            elide: Text.ElideRight
        }

        Label {
            id: subtitle
            font.pixelSize: 22
            font.family: "Nokia Pure Text Light"
            color: "#505050"
            visible: text != ""
            width: root.width - UI.DEFAULT_SPACING - icon.width
            elide: Text.ElideRight
        }
    }

    ToolIcon {
        id: icon
        iconId: "toolbar-next"
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
