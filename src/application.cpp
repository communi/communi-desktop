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
#include "connectioninfo.h"
#include "settingsmodel.h"
#include "sharedtimer.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QIcon>
#include <QFile>
#include <Irc>

QByteArray Application::Private::encoding("ISO-8859-15");

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

    qRegisterMetaTypeStreamOperators<ConnectionInfo>("ConnectionInfo");
    qRegisterMetaTypeStreamOperators<ConnectionInfos>("ConnectionInfos");

    QSettings settings;
    if (arguments().contains("-reset"))
        settings.clear();
    if (arguments().contains("-reset-settings"))
        settings.remove("settings");
    if (arguments().contains("-reset-connections"))
        settings.remove("connections");

    if (settings.contains("settings")) {
        QVariant value = settings.value("settings");
        if (value.canConvert<QVariantMap>())
            Application::settings()->setValues(value.toMap());
    }

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
    settings.setValue("settings", Application::settings()->values());
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

SettingsModel* Application::settings()
{
    static SettingsModel model;
    if (model.rowCount() == 0)
        model.setValues(defaultSettings());
    return &model;
}

QVariantMap Application::defaultSettings()
{
    QVariantMap settings;

    settings["ui.font"] = QFont();
    settings["ui.mute"] = false;
    settings["ui.theme"] = "light";
    settings["ui.scrollback"] = 0;

    settings["session.reconnectDelay"] = 15;
    settings["session.lagTimerInterval"] = 60;
    settings["session.channelRejoinBatch"] = 10;

    settings["formatting.timeStamp"] = "[hh:mm:ss]";
    settings["formatting.hideUserHosts"] = false;

#ifdef Q_OS_MAC
    QString navigate("Ctrl+Alt+%1");
    QString nextActive("Shift+Ctrl+Alt+%1");
#else
    QString navigate("Alt+%1");
    QString nextActive("Shift+Alt+%1");
#endif

    settings["shortcuts.previousView"] = navigate.arg("Up");
    settings["shortcuts.nextView"] = navigate.arg("Down");
    settings["shortcuts.collapseView"] = navigate.arg("Left");
    settings["shortcuts.expandView"] = navigate.arg("Right");
    settings["shortcuts.previousActiveView"] = nextActive.arg("Up");
    settings["shortcuts.nextActiveView"] = nextActive.arg("Down");
    settings["shortcuts.mostActiveView"] = "Ctrl+L";
    settings["shortcuts.searchView"] = "Ctrl+S";

    // TODO:
//    settings["messages.joins"] = true;
//    settings["messages.parts"] = true;
//    settings["messages.nicks"] = true;
//    settings["messages.modes"] = true;
//    settings["messages.kicks"] = true;
//    settings["messages.quits"] = true;
//    settings["messages.topics"] = true;

    settings["themes.light.background"] = QColor("#ffffff");
    settings["themes.light.message"] = QColor("#000000");
    settings["themes.light.event"] = QColor("#808080");
    settings["themes.light.notice"] = QColor("#a54242");
    settings["themes.light.action"] = QColor("#8b388b");
    settings["themes.light.highlight"] = QColor("#ff4040");
    settings["themes.light.timestamp"] = QColor("#808080");
    settings["themes.light.link"] = QColor("#4040ff");

    settings["aliases.chanserv"] = "MSG ChanServ $*";
    settings["aliases.nickserv"] = "MSG NickServ $*";

    return settings;
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
    SettingsModel model;
    model.setValues(settings()->values());
    SettingsWizard wizard(&model, activeWindow());
    if (wizard.exec())
        settings()->setValues(model.values());
}
