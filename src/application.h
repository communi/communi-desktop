/*
* Copyright (C) 2008-2013 The Communi Project
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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QVariantMap>
#include <QDir>

class SettingsModel;

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int& argc, char* argv[]);
    ~Application();

    static QString applicationSlogan();

    static QByteArray encoding();
    static void setEncoding(const QByteArray& encoding);

    static SettingsModel* settings();
    static QVariantMap defaultSettings();
    static QDir dataDir();

public slots:
    static void aboutApplication();
    static void showSettings();
    static void applySettings(QVariantMap values = QVariantMap());

private:
    struct Private {
        static QByteArray encoding;
    };
};

#endif // APPLICATION_H
