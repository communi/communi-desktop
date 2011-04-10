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

#include "shortcutmanager.h"
#include "application.h"
#include "settings.h"

#include <QShortcut>
#include <QWidget>

QHash<QWidget*, ShortcutManager*> ShortcutManager::ShortcutManagerData::instances;

ShortcutManager* ShortcutManager::instance(QWidget* window)
{
    if (!ShortcutManagerData::instances.contains(window))
        ShortcutManagerData::instances[window] = new ShortcutManager(window);
    return ShortcutManagerData::instances.value(window);
}

ShortcutManager::ShortcutManager(QWidget* window) : QObject(window)
{
    Q_ASSERT(window);
    d.shortcuts[NavigateUp] = new QShortcut(window);
    d.shortcuts[NavigateDown] = new QShortcut(window);
    d.shortcuts[NavigateLeft] = new QShortcut(window);
    d.shortcuts[NavigateRight] = new QShortcut(window);

    connect(qApp, SIGNAL(settingsChanged(Settings)), this, SLOT(applySettings(Settings)));
    applySettings(Application::settings());
}

ShortcutManager::~ShortcutManager()
{
    ShortcutManagerData::instances.remove(static_cast<QWidget*>(parent()));
}

QShortcut* ShortcutManager::shortcut(ShortcutType type) const
{
    return d.shortcuts.value(type);
}

void ShortcutManager::applySettings(const Settings& settings)
{
    d.shortcuts.value(NavigateUp)->setKey(QKeySequence(settings.shortcuts.value(Settings::TabUp)));
    d.shortcuts.value(NavigateDown)->setKey(QKeySequence(settings.shortcuts.value(Settings::TabDown)));
    d.shortcuts.value(NavigateLeft)->setKey(QKeySequence(settings.shortcuts.value(Settings::TabLeft)));
    d.shortcuts.value(NavigateRight)->setKey(QKeySequence(settings.shortcuts.value(Settings::TabRight)));
}
