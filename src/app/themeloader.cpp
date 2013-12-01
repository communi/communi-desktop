/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "themeloader.h"
#include <QFileInfo>
#include <QDebug>
#include <QDir>

ThemeLoader::ThemeLoader(QObject* parent) : QObject(parent)
{
#ifdef Q_OS_MAC
    QDir dir = QDir::current();
    if (dir.dirName() == "MacOS" && dir.cd("../Resources/themes")) {
        foreach (const QString& theme, dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs)) {
            if (dir.cd(theme)) {
                foreach (const QString& fn, dir.entryList(QStringList("*.theme"), QDir::Files)) {
                    d.themes.append(theme);
                    d.paths.insert(theme, dir.filePath(fn));
                }
                dir.cdUp();
            }
        }
    }
#else
    d.themes.append("cute");
    d.paths.insert("cute", ":/themes/cute/cute.theme");

    d.themes.append("dark");
    d.paths.insert("dark", ":/themes/dark/dark.theme");
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
    ThemeInfo info;
    info.load(d.paths.value(name));
    return info;
}
