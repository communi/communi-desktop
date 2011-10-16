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

#include "application.h"
#include "settingswizard.h"
#include "sharedtimer.h"
#include "connection.h"
#include <QMessageBox>
#include <QSettings>
#include <QIcon>
#include <Irc>

QByteArray Application::ApplicationData::encoding;
Settings Application::ApplicationData::settings;

Application::Application(int& argc, char* argv[]) : QApplication(argc, argv)
{
    setApplicationName("Communi");
    setOrganizationName("Communi");
    setApplicationVersion(Irc::version());

    QIcon icon;
    icon.addFile(":/resources/icons/16x16/communi.png");
    icon.addFile(":/resources/icons/26x26/communi.png");
    icon.addFile(":/resources/icons/32x32/communi.png");
    icon.addFile(":/resources/icons/48x48/communi.png");
    icon.addFile(":/resources/icons/64x64/communi.png");
    icon.addFile(":/resources/icons/128x128/communi.png");
    setWindowIcon(icon);

    qRegisterMetaTypeStreamOperators<Settings>("Settings");
    qRegisterMetaTypeStreamOperators<Connection>("Connection");
    qRegisterMetaTypeStreamOperators<Connections>("Connections");

    QSettings settings;
    ApplicationData::settings = settings.value("settings").value<Settings>();

    SharedTimer::instance()->setInterval(500);
}

Application::~Application()
{
    QSettings settings;
    settings.setValue("settings", ApplicationData::settings);
}

QString Application::applicationSlogan()
{
    return tr("%1 is a simple and light-weight cross-platform IRC client.").arg(applicationName());
}

QByteArray Application::encoding()
{
    return ApplicationData::encoding;
}

void Application::setEncoding(const QByteArray& encoding)
{
    ApplicationData::encoding = encoding;
}

Settings Application::settings()
{
    return ApplicationData::settings;
}

void Application::setSettings(const Settings& settings)
{
    if (ApplicationData::settings != settings)
    {
        ApplicationData::settings = settings;
        QMetaObject::invokeMethod(qApp, "settingsChanged", Q_ARG(Settings, settings));
    }
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
        "<p><small>Copyright (C) 2008-2011 J-P Nurmi &lt;"
        "<a href='mailto:jpnurmi@gmail.com'>jpnurmi@gmail.com</a>&gt;</small></p>")
            .arg(tr("About %1").arg(applicationName()))
            .arg(applicationSlogan())
            .arg(organizationDomain())
        );
}

void Application::aboutOxygen()
{
    QMessageBox::about(
#ifdef Q_WS_MAC
        0,
#else
        activeWindow(),
#endif // Q_WS_MAC
        "Oxygen", QString(
        "<h3>%1</h3>"
        "<p><a href='http://www.oxygen-icons.org/'>http://www.oxygen-icons.org/</a></p>"
        ).arg(tr("About %1").arg("Oxygen")));
}

void Application::showSettings()
{
    SettingsWizard wizard(activeWindow());
    wizard.setSettings(settings());
    if (wizard.exec())
        setSettings(wizard.settings());
}
