import QtQuick 1.1
import Communi 1.0
import com.nokia.meego 1.0

PageStackWindow {
    id: window

    initialPage: MainPage {
        title: qsTr("Communi")
        tools: ToolBarLayout {
            id: tools
            visible: sheet.status == DialogStatus.Closed
            ToolIcon {
                iconId: "toolbar-add"
                anchors.right: parent.right
                onClicked: SessionModel.length ? menu.open() : connectionSheet.open()
            }
        }

        Menu {
            id: menu
            MenuLayout {
                MenuItem {
                    text: qsTr("Add connection")
                    onClicked: connectionSheet.open()
                }
                MenuItem {
                    text: qsTr("Join channel")
                    onClicked: channelSheet.open()
                }
                MenuItem {
                    text: qsTr("Open query")
                    onClicked: querySheet.open()
                }
            }
        }

        IrcCommand {
            id: ircCommand
        }

        ConnectionSheet {
            id: connectionSheet

            Component {
                id: sessionComponent
                Session {
                    id: session
                    property string channel
                    onConnected: {
                        if (channel.length) {
                            var cmd = ircCommand.createJoin(channel);
                            session.sendCommand(cmd);
                        }
                    }
                }
            }

            onAccepted: {
                var session = sessionComponent.createObject(window);
                session.userName = "communi";
                session.nickName = connectionSheet.name;
                session.realName = connectionSheet.name;
                session.host = connectionSheet.host;
                session.port = connectionSheet.port;
                session.channel = connectionSheet.channel;
                session.password = connectionSheet.password;
                session.secure = connectionSheet.secure;
                SessionManager.addSession(session);
                session.open();
            }
        }

        ChatSheet {
            id: channelSheet
            value: "#"
            titleText: qsTr("Join channel")
            description: qsTr("Channel")
            onAccepted: {
                var item = SessionModel[channelSheet.session];
                if (item) {
                    var cmd = ircCommand.createJoin(channelSheet.value);
                    item.session.sendCommand(cmd);
                }
            }
        }

        ChatSheet {
            id: querySheet
            titleText: qsTr("Open query")
            description: qsTr("User")
            onAccepted: {
                var item = SessionModel[querySheet.session];
                if (item)
                    item.addChild(querySheet.value);
            }
        }
    }
}
