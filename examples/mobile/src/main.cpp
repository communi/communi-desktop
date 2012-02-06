/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication::setApplicationName("Communi");
    QApplication::setOrganizationName("Communi");
    QApplication::setApplicationVersion(Irc::version());

    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QScopedPointer<QmlApplicationViewer> viewer(QmlApplicationViewer::create());

    QString appName = QObject::tr("%1 %2 for %3").arg(QApplication::applicationName())
                                                 .arg(QApplication::applicationVersion());
#ifdef Q_OS_SYMBIAN
    viewer->rootContext()->setContextProperty("ApplicationName", appName.arg(QObject::tr("Symbian")));
#else
    viewer->rootContext()->setContextProperty("ApplicationName", appName.arg(QObject::tr("MeeGo")));
    viewer->engine()->addImportPath("/opt/communi/imports");
#endif

    qmlRegisterType<MessageFormatter>("Communi", 1, 0, "MessageFormatter");
    qmlRegisterType<MessageHandler>("Communi", 1, 0, "MessageHandler");

    Settings* settings = new Settings(viewer->rootContext());
    viewer->rootContext()->setContextProperty("Settings", settings);

    CommandParser* parser = new CommandParser(viewer->rootContext());
    viewer->rootContext()->setContextProperty("CommandParser", parser);

    Completer* completer = new Completer(viewer->rootContext());
    viewer->rootContext()->setContextProperty("Completer", completer);

    SessionManager* manager = new SessionManager(viewer->rootContext());
    viewer->rootContext()->setContextProperty("SessionManager", manager);
    qmlRegisterUncreatableType<SessionItem>("Communi", 1, 0, "SessionItem", "");
    qmlRegisterType<Session>("Communi", 1, 0, "Session");

    viewer->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
#ifdef Q_OS_SYMBIAN
    viewer->setMainQmlFile(QLatin1String("qml/symbian/main.qml"));
#else
    viewer->setMainQmlFile(QLatin1String("qml/meego/main.qml"));
#endif
    viewer->showExpanded();

    return app->exec();
}
