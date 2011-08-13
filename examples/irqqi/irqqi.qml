import QtQuick 1.0
import org.gitorious.communi 1.0

Rectangle {
    width: 640
    height: 480

    Flickable {
        id: flickable
        anchors.fill: parent
        anchors.leftMargin: scrollbar.width
        contentHeight: text.height
        Text {
            id: text
            wrapMode: Text.WordWrap
            width: parent.width
        }
    }

    Rectangle {
        id: scrollbar
        anchors.right: flickable.right
        y: flickable.visibleArea.yPosition * flickable.height
        width: 2
        height: flickable.visibleArea.heightRatio * flickable.height
        color: "black"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            session.open();
            enabled = false;
        }
        Text {
            text: "Click!"
            visible: parent.enabled
            anchors.centerIn: parent
        }
    }

    IrcSession {
        id: session

        port: 6667
        host: "irc.freenode.net"

        userName: "jipsu"
        nickName: "jipsukka"
        realName: "Da Jipsuh"

        onConnecting: console.log("connecting...")
        onConnected: {
            console.log("connected...");
            var cmd = session.createJoinCommand("#communi", "");
            session.sendCommand(cmd);
            cmd.destroy();
        }
        onDisconnected: console.log("disconnected...")
        onMessageReceived: {
            text.text += "<"+message.prefix+"> "+message.command+" "+message.parameters.join(" ")+"\n";
        }
    }
}
