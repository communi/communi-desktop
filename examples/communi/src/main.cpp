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

#include "application.h"
#include "mainwindow.h"
#include <QNetworkProxy>
#include <QUrl>

#ifdef Q_WS_WIN
#include "qtwin.h"
#endif

int main (int argc, char* argv[])
{
    Application app(argc, argv);

    QStringList args = app.arguments();
    int index = args.indexOf("-proxy");
    if (index != -1)
    {
        QUrl url(args.value(index + 1));
        if (!url.isEmpty())
        {
            QNetworkProxy proxy;
            if (url.scheme() == "http")
                proxy.setType(QNetworkProxy::HttpProxy);
            else if (url.scheme() == "socks5")
                proxy.setType(QNetworkProxy::Socks5Proxy);
            proxy.setHostName(url.host());
            if (url.port() != -1)
                proxy.setPort(url.port());
            if (!url.userName().isEmpty())
                proxy.setUser(url.userName());
            if (!url.password().isEmpty())
                proxy.setPassword(url.password());
            QNetworkProxy::setApplicationProxy(proxy); 
        }
    }

    MainWindow window;
#ifdef Q_WS_WIN
    if (QtWin::isCompositionEnabled()) {
        QtWin::extendFrameIntoClientArea(&window);
        window.setContentsMargins(0, 0, 0, 0);
    }
#endif
    window.show();
    return app.exec();
}
