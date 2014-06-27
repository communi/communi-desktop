/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "themeloader.h"
#include <QApplication>
#include <QFileInfo>
#include <QDebug>

ThemeLoader::ThemeLoader(QObject* parent) : QObject(parent)
{
    ThemeInfo info;
    if (!info.load(":/themes/cute/cute.theme"))
        qWarning() << "Failed to load cute.theme";
    d.themes.append(info.name());
    d.infos.insert(info.name(), info);

#if defined(Q_OS_MAC)
    QDir dir(QApplication::applicationDirPath());
    if (dir.dirName() == "MacOS" && dir.cd("../Resources/themes"))
        load(dir);
#elif defined(Q_OS_WIN)
    QDir dir(QApplication::applicationDirPath());
    if (dir.cd("themes") || (dir.cdUp() && dir.cd("themes")))
        load(dir);
#elif defined(Q_OS_UNIX)
    QDir sys("/usr/share/themes/communi");
    if (sys.exists())
        load(sys);
    QDir home = QDir::home();
    if (home.cd(".local/share/themes/communi"))
        load(home);
    QDir dev(QApplication::applicationDirPath());
    if (dev.cdUp() && dev.cd("themes"))
        load(dev);
#endif
}

ThemeLoader* ThemeLoader::instance()
{
    static ThemeLoader loader;
    return &loader;
}

QStringList ThemeLoader::themes() const
{
    return d.themes;
}

ThemeInfo ThemeLoader::theme(const QString& name) const
{
    return d.infos.value(name, d.infos.value("Cute"));
}

void ThemeLoader::load(QDir dir)
{
    QStringList dirs = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    foreach (const QString& sd, dirs) {
        if (dir.cd(sd)) {
            QStringList files = dir.entryList(QStringList("*.theme"), QDir::Files);
            foreach (const QString& fn, files) {
                ThemeInfo info;
                if (info.load(dir.filePath(fn))) {
                    if (!d.infos.contains(info.name())) {
                        d.themes.append(info.name());
                        d.infos.insert(info.name(), info);
                    }
                }
            }
            dir.cdUp();
        }
    }
}
