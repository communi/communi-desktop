import QtQuick 1.1
import com.nokia.meego 1.0
import "UIConstants.js" as UI

Page {
    id: page

    property alias title: label.text
    default property alias content: content.data

    Rectangle {
        id: background

        anchors.fill: parent
        color: "#e0e1e2"
    }

    Item {
        id: content

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    Rectangle {
        id: header

        width: parent.width
        height: screen.currentOrientation == Screen.Landscape ? 46 : 72
        color: "#4591ff"

        Label {
            id: label

            anchors {
                left: parent.left
                right: parent.right
                leftMargin: UI.PAGE_MARGIN
                rightMargin: UI.PAGE_MARGIN
            }

            color: "white"
            elide: Text.ElideRight
            font.pixelSize: 32
            font.family: "Nokia Pure Text Light"
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
