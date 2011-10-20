import QtQuick 1.1
import com.nokia.meego 1.0
import "UIConstants.js" as UI

CommonPage {
    id: root

    ListView {
        id: listView
        anchors.fill: parent
        model: SessionModel
        delegate: Column {
            width: parent.width
            ListItem {
                onClicked: chatPage.push(modelData)
            }
            Repeater {
                model: modelData.childItems
                ListItem {
                    onClicked: chatPage.push(modelData)
                }
            }
            Rectangle {
                color: "#b2b2b4"
                height: 1
                visible: index < listView.count - 1
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: UI.PAGE_MARGIN
                    rightMargin: UI.PAGE_MARGIN
                }
            }
        }
    }

    ScrollDecorator {
        flickableItem: listView
    }

    ChatPage {
        id: chatPage
        function push(data) {
            modelData = data;
            root.pageStack.push(chatPage);
        }
        onStatusChanged: {
            modelData.current = (status == PageStatus.Active);
            if (status == PageStatus.Inactive) modelData = null;
        }
    }
}
