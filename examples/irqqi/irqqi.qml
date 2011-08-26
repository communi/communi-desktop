import QtQuick 1.0
import QtDesktop 0.1
import org.gitorious.communi 1.0
import org.gitorious.communi.examples 1.0

Window {
    id: window
    width: 640
    height: 480

    Dialog {
        id: dialog
        modal: true
        onConnect: {
            mainPage.title = dialog.host;
            session.host = dialog.host;
            session.userName = dialog.name;
            session.nickName = dialog.name;
            session.realName = dialog.name;
            session.open();
            dialog.visible = false;
        }
    }
    Component.onCompleted: dialog.visible = true;

    TabFrame {
        id: tabFrame
        anchors.fill: parent
        tabbar: TabBar { }

        Page {
            id: mainPage
            title: qsTr("Home")
            onSendMessage: session.sendMessage(receiver, message)
        }
    }

    Component {
        id: pageComponent
        Page { }
    }

    MessageHandler {
        id: handler
        session: session
        currentReceiver: tabFrame.count ? tabFrame.tabs[tabFrame.current] : null
        defaultReceiver: mainPage
        onReceiverToBeAdded: {
            var page = pageComponent.createObject(tabFrame.stack);
            page.sendMessage.connect(session.sendMessage);
            page.title = name;
            tabFrame.current = tabFrame.count - 1;
            handler.addReceiver(name, page);
        }
        onReceiverToBeRemoved: {
            var page = handler.getReceiver(name);
            tabFrame.current -= 1;
            page.destroy();
        }
    }

    IrcSession {
        id: session

        onConnecting: console.log("connecting...")
        onConnected: console.log("connected...")
        onDisconnected: Qt.quit();

        function sendMessage(receiver, message) {
            var cmd = CommandParser.parseCommand(receiver, message, session);
            session.sendCommand(cmd);
            cmd.destroy();
        }
    }
}
