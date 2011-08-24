import QtQuick 1.0
import QtDesktop 0.1
import org.gitorious.communi 1.0
import org.gitorious.communi.examples 1.0

Rectangle {
    id: window
    width: 640
    height: 480

    TabFrame {
        id: tabFrame
        anchors.fill: parent
        tabbar: TabBar { }

        MainPage {
            id: mainPage
            title: qsTr("Home")
            onConnect: {
                mainPage.title = host;
                session.host = host;
                session.userName = name;
                session.nickName = name;
                session.realName = name;
                session.open();
            }
        }
    }

    Component {
        id: pageComponent
        Page { }
    }

    MessageHandler {
        id: handler
        session: session
        defaultReceiver: mainPage
        onReceiverToBeAdded: {
            var page = pageComponent.createObject(tabFrame.stack);
            tabFrame.current = tabFrame.count - 1;
            page.title = name;
            handler.addReceiver(name, page);
        }
    }

    IrcCommand {
        id: command
    }

    IrcSession {
        id: session

        onConnecting: console.log("connecting...")
        onConnected: {
            console.log("connected...");
            var cmd = command.createJoin("#communi", "");
            session.sendCommand(cmd);
            cmd.destroy();
        }
        onDisconnected: console.log("disconnected...")
        onMessageReceived: {
            console.log(MessageFormatter.formatMessage(message));
        }
    }
}
