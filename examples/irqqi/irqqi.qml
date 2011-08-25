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
                mainPage.title = mainPage.host;
                session.host = mainPage.host;
                session.userName = mainPage.name;
                session.nickName = mainPage.name;
                session.realName = mainPage.name;
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
        currentReceiver: tabFrame.count ? tabFrame.tabs[tabFrame.current] : null
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
            if (mainPage.channel != "") {
                var cmd = command.createJoin(mainPage.channel, "");
                session.sendCommand(cmd);
                cmd.destroy();
            }
            var cmd = command.createWhois(session.nickName);
            session.sendCommand(cmd);
            cmd.destroy();
        }
        onDisconnected: console.log("disconnected...")
    }
}
