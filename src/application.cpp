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

#include "application.h"
#include "settingswizard.h"
#include "sharedtimer.h"
#include "connectioninfo.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QSettings>
#include <QIcon>
#include <QFile>
#include <Irc>

QByteArray Application::Private::encoding("ISO-8859-15");
Settings Application::Private::settings;

Application::Application(int& argc, char* argv[]) : QApplication(argc, argv)
{
    setApplicationName("Communi");
    setOrganizationName("Communi");
    setApplicationVersion(Irc::version());

    QIcon icon;
    icon.addFile(":/resources/icons/16x16/communi.png");
    icon.addFile(":/resources/icons/24x24/communi.png");
    icon.addFile(":/resources/icons/32x32/communi.png");
    icon.addFile(":/resources/icons/48x48/communi.png");
    icon.addFile(":/resources/icons/64x64/communi.png");
    icon.addFile(":/resources/icons/128x128/communi.png");
    setWindowIcon(icon);

    qRegisterMetaTypeStreamOperators<Settings>("Settings");
    qRegisterMetaTypeStreamOperators<ConnectionInfo>("ConnectionInfo");
    qRegisterMetaTypeStreamOperators<ConnectionInfos>("ConnectionInfos");

    QSettings settings;
    if (arguments().contains("-reset"))
        settings.clear();
    Private::settings = settings.value("settings").value<Settings>();

    QFile file(":resources/stylesheet.css");
    if (file.open(QFile::ReadOnly | QIODevice::Text))
        setStyleSheet(QString::fromUtf8(file.readAll()));

    QDir dir = dataDir();
    if (dir.exists() || dir.mkpath(".")) {
        QString filePath = dir.filePath("notify.mp3");
        if (!QFile::exists(filePath))
            QFile::copy(":/resources/notify.mp3", filePath);
    }
}

Application::~Application()
{
    QSettings settings;
    settings.setValue("settings", Private::settings);
}

QString Application::applicationSlogan()
{
    return tr("%1 is a simple and light-weight cross-platform IRC client.").arg(applicationName());
}

QByteArray Application::encoding()
{
    return Private::encoding;
}

void Application::setEncoding(const QByteArray& encoding)
{
    Private::encoding = encoding;
}

Settings Application::settings()
{
    return Private::settings;
}

void Application::setSettings(const Settings& settings)
{
    if (Private::settings != settings) {
        Private::settings = settings;
        QMetaObject::invokeMethod(qApp, "settingsChanged", Q_ARG(Settings, settings));
    }
}

QDir Application::dataDir()
{
    return QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
}

void Application::aboutApplication()
{
    QMessageBox::about(
#ifdef Q_WS_MAC
        0,
#else
        activeWindow(),
#endif // Q_WS_MAC
        applicationName(), QString(
            "<h3>%1</h3>"
            "<p>%2</p>"
            "<p><a href='%3'>%3</a></p>"
            "<p><small>Copyright (C) 2008-2013 The Communi Project</small></p>")
        .arg(tr("About %1").arg(applicationName()))
        .arg(applicationSlogan())
        .arg(organizationDomain())
    );
}

void Application::showSettings()
{
    SettingsWizard wizard(activeWindow());
    wizard.setSettings(settings());
    if (wizard.exec())
        setSettings(wizard.settings());
}
