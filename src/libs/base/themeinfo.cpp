/*
  Copyright (C) 2008-2015 The Communi Project

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

#include "themeinfo.h"
#include <QStringList>
#include <QSettings>
#include <QFileInfo>
#include <QFile>
#include <QDir>

bool ThemeInfo::isValid() const
{
    return !d.name.isEmpty();
}

QString ThemeInfo::name() const
{
    return d.name;
}

QString ThemeInfo::author() const
{
    return d.author;
}

QString ThemeInfo::version() const
{
    return d.version;
}

QString ThemeInfo::description() const
{
    return d.description;
}

QString ThemeInfo::style() const
{
    return d.style;
}

QString ThemeInfo::gtkTheme() const
{
    return d.gtkTheme;
}

QString ThemeInfo::font() const
{
    return d.font;
}

QString ThemeInfo::path() const
{
    return d.path;
}

static QString readFile(const QDir& dir, const QString& fileName)
{
    QFile file;
    if (QFileInfo(fileName).isRelative())
        file.setFileName(dir.filePath(fileName));
    else
        file.setFileName(fileName);
    if (file.open(QFile::ReadOnly | QIODevice::Text))
        return QString::fromUtf8(file.readAll());
    return QString();
}

bool ThemeInfo::load(const QString& filePath)
{
    QSettings settings(filePath, QSettings::IniFormat);
    d.path = QFileInfo(filePath).path();

    QStringList groups = settings.childGroups();
    if (groups.contains("Theme")) {
        settings.beginGroup("Theme");
        d.name = settings.value("name").toString();
        d.author = settings.value("author").toString();
        d.version = settings.value("version").toString();
        d.description = settings.value("description").toString();
        d.style = readFile(QFileInfo(filePath).dir(), settings.value("style").toString());
        d.gtkTheme = settings.value("gtk-theme").toString();
        d.font = settings.value("font").toString();
        settings.endGroup();
    }
    return isValid();
}
