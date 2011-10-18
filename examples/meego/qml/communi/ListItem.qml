import QtQuick 1.1
import com.nokia.meego 1.0
import "UIConstants.js" as UI

Item {
    id: root

    property alias title: title.text
    property alias subtitle: subtitle.text
    property bool highlighted: false
    property bool busy: false
    property int unread: 0

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
            right: loader.left
            leftMargin: UI.PAGE_MARGIN
            rightMargin: UI.DEFAULT_SPACING
            verticalCenter: parent.verticalCenter
        }

        Label {
            id: title
            font.bold: true
            font.pixelSize: 26
            font.family: "Nokia Pure Text"
            color: highlighted ? "red" : "#282828"
            width: parent.width
            elide: Text.ElideRight
        }

        Label {
            id: subtitle
            font.pixelSize: 22
            font.family: "Nokia Pure Text Light"
            color: "#505050"
            width: parent.width
            elide: Text.ElideRight
        }
    }

    Loader {
        id: loader
        width: 32
        height: 32
        anchors.right: parent.right
        anchors.rightMargin: UI.PAGE_MARGIN
        anchors.verticalCenter: parent.verticalCenter

        sourceComponent: root.busy ? indicator : root.unread ? badge : icon

        Component {
            id: indicator
            BusyIndicator { running: root.busy }
        }

        Component {
            id: badge
            Image {
                source: "squircle.png"
                Label {
                    id: unread
                    color: "white"
                    text: root.unread
                    anchors.fill: parent
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Component {
            id: icon
            ToolIcon { iconId: "toolbar-next" }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
