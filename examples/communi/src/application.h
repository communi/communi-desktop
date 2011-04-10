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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "settings.h"
#include "command.h"

class Session;

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int& argc, char* argv[]);
    ~Application();

    static QString applicationSlogan();

    static Settings settings();
    static void setSettings(const Settings& settings);

    static QList<Session*> sessions();
    static void setSessions(const QList<Session*>& sessions);

    static QStringList commandCategories();
    static Commands commands(const QString& category = QString());
    static void setCommands(const Commands& commands);

public slots:
    static void aboutApplication();
    static void aboutOxygen();
    static void showSettings();

signals:
    void settingsChanged(const Settings& settings);
    void commandsChanged(const Commands& commands);

private:
    struct ApplicationData
    {
        static Settings settings;
        static QList<Session*> sessions;
        static Commands commands;
        static QStringList categories;
    };
};

#endif // APPLICATION_H
