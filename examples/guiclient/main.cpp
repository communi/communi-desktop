/*
* Copyright (C) 2008 J-P Nurmi jpnurmi@gmail.com
* Copyright (C) 2008 Adam Higerd ahigerd@libqxt.org
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* $Id$
*/

#include <QtGui>
#include "mainwindow.h"

int main (int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("libircclient-qt.googlecode.com");
    app.setApplicationName("guiclient");
    MainWindow window;
    window.resize(800, 600);
    window.show();
    return app.exec();
}
