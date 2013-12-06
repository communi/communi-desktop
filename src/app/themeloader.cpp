/*
 * Copyright (C) 2008-2013 The Communi Project
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
    d.themes.append("cute");
    d.paths.insert("cute", ":/themes/cute/cute.theme");

#if defined(Q_OS_MAC)
    QDir dir = QDir::current();
    if (dir.dirName() == "MacOS" && dir.cd("../Resources/themes"))
        load(dir);
#elif defined(Q_OS_WIN)
    QDir dir(QApplication::applicationDirPath());
    if (dir.cd("themes"))
        load(dir);
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

void ThemeLoader::load(QDir dir)
{
    QStringList dirs = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    foreach (const QString& sd, dirs) {
        if (dir.cd(sd)) {
            QStringList files = dir.entryList(QStringList("*.theme"), QDir::Files);
            foreach (const QString& fn, files) {
                d.themes.append(sd);
                d.paths.insert(sd, dir.filePath(fn));
            }
            dir.cdUp();
        }
    }
}
