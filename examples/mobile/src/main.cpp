/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include <QApplication>
#include "qmlapplicationviewer.h"

#include <QtPlugin>
#include <QtDeclarative>
#include "messageformatter.h"
#include "messagehandler.h"
#include "commandparser.h"
#include "completer.h"
#include "sessionmanager.h"
#include "sessionitem.h"
#include "session.h"
#include "settings.h"
#include <irc.h>

#ifdef COMMUNI_STATIC_ICU_PLUGIN
    Q_IMPORT_PLUGIN(icuplugin)
#endif

#ifdef COMMUNI_STATIC_UCHARDET_PLUGIN
    Q_IMPORT_PLUGIN(uchardetplugin)
#endif

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

Q_DECL_EXPORT int main(int argc, char* argv[])
{
    QApplication::setApplicationName("Communi");
    QApplication::setOrganizationName("Communi");
    QApplication::setApplicationVersion(QString("%1.%2").arg(Irc::version())
                                                        .arg(STRINGIFY(COMMUNI_EXAMPLE_REVISION)));

    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QScopedPointer<QmlApplicationViewer> viewer(QmlApplicationViewer::create());

    QString appName = QObject::tr("%1 %2 for %3").arg(QApplication::applicationName())
                      .arg(QApplication::applicationVersion())
                      .arg(STRINGIFY(COMMUNI_PLATFORM));
    viewer->rootContext()->setContextProperty("ApplicationName", appName);
#ifdef COMMUNI_IMPORT_PATH
    viewer->engine()->addImportPath(STRINGIFY(COMMUNI_IMPORT_PATH));
#endif
#ifdef COMMUNI_PLUGIN_PATH
    QCoreApplication::addLibraryPath(STRINGIFY(COMMUNI_PLUGIN_PATH));
#endif

    qmlRegisterType<MessageFormatter>("Communi", 1, 0, "MessageFormatter");
    qmlRegisterType<MessageHandler>("Communi", 1, 0, "MessageHandler");

    viewer->rootContext()->setContextProperty("Settings", Settings::instance());

    QScopedPointer<CommandParser> parser(new CommandParser(viewer->rootContext()));
    viewer->rootContext()->setContextProperty("CommandParser", parser.data());

    QScopedPointer<Completer> completer(new Completer(viewer->rootContext()));
    viewer->rootContext()->setContextProperty("Completer", completer.data());

    QScopedPointer<SessionManager> manager(new SessionManager(appName, viewer->rootContext()));
    viewer->rootContext()->setContextProperty("SessionManager", manager.data());

    foreach (const QString& arg, app->arguments()) {
        // irc:[ //[ <host>[:<port>] ]/[<target>]
        QRegExp rx("irc:(?://([^:/]+)(?::(\\d+))?)?(?:/([^/]*))?");
        if (rx.exactMatch(arg)) {
            QString host = rx.cap(1);
            QString port = rx.cap(2);
            QString target = rx.cap(3);

            ConnectionInfo info;
            info.host = !host.isEmpty() ? host : "localhost";
            info.port = !port.isEmpty() ? port.toInt() : 6667;
            info.nick = Settings::instance()->name();
            info.user = Settings::instance()->user();
            info.real = Settings::instance()->real();
            if (!target.isEmpty()) {
                ChannelInfo channel;
                channel.channel = target;
                info.channels += channel;
            }

            Session* session = Session::fromConnection(info);
            manager->addSession(session);
            session->reconnect();
        }
    }

    qmlRegisterUncreatableType<SessionItem>("Communi", 1, 0, "SessionItem", "");
    qmlRegisterType<Session>("Communi", 1, 0, "Session");

    viewer->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer->setMainQmlFile(STRINGIFY(COMMUNI_QML_DIR)"/main.qml");
    viewer->showExpanded();

    return app->exec();
}
