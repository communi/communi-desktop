/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#include <QtGui>
#include <QtDeclarative>
#include <IrcSession>
#include "messageformatter.h"
#include "messagehandler.h"
#include "commandparser.h"

QML_DECLARE_TYPE(MessageHandler)

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QDeclarativeView view;
    QDeclarativeContext* context = view.engine()->rootContext();
    CommandParser parser;
    MessageFormatter formatter;
    context->setContextProperty("CommandParser", &parser);
    context->setContextProperty("MessageFormatter", &formatter);
    qmlRegisterType<MessageHandler>("org.gitorious.communi.examples", 1, 0, "MessageHandler");

    app.connect(view.engine(), SIGNAL(quit()), SLOT(quit()));
    view.setSource(QUrl::fromLocalFile("irqqi.qml"));
    view.show();

    return app.exec();
}
