/*
  Copyright (C) 2008-2014 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

    QDir inst(COMMUNI_INSTALL_THEMES);
    if (inst.exists())
        load(inst);

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
    if (sys != inst && sys.exists())
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
