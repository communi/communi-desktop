/*
* Copyright (C) 2008-2014 The Communi Project
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

#include "gnomemenu.h"
#include "x11helper.h"
#include <QCoreApplication>
#include <QDebug>

#ifdef COMMUNI_HAVE_GIO
#undef signals
#include <glib-2.0/gio/gio.h>
#define signals Q_SIGNALS

struct GnomeMenuUserData
{
    QObject* receiver;
    const char* slot;
};

struct GnomeMenuPrivate
{
    GDBusConnection* dbusSession;
    GSimpleActionGroup* group;
    GMenu* menu;
    guint exportedActionsId;
    guint exportedMenuId;

    QList<GActionEntry> entries;
    QList<GnomeMenuUserData> entriesUserDatas;

    // These things need to remain in memory for a while, so let's put them here to
    // have them automatically cleaned up when AppMenuBuilderPrivate is destroyed
    QList<QByteArray> actionNameBytes;
};

GnomeMenu::GnomeMenu(QObject* parent) : QObject(parent), d(new GnomeMenuPrivate)
{
    d->dbusSession = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, nullptr);
    d->group = nullptr;
    d->menu = g_menu_new();
    d->exportedActionsId = 0;
    d->exportedMenuId = 0;
}

GnomeMenu::~GnomeMenu()
{
    if (d->exportedActionsId)
        g_dbus_connection_unexport_action_group(d->dbusSession, d->exportedActionsId);
    if (d->exportedMenuId)
        g_dbus_connection_unexport_menu_model(d->dbusSession, d->exportedMenuId);

    if (d->dbusSession)
        g_object_unref(G_OBJECT(d->dbusSession));
    if (d->group)
        g_object_unref(G_OBJECT(d->group));
    if (d->menu)
        g_object_unref(G_OBJECT(d->menu));
}

void GnomeMenu::addSimpleItem(const QByteArray& id, const QString& caption, QObject* receiver, const char* slot)
{
    GActionEntry entry;
    memset(&entry, 0, sizeof(GActionEntry));
    d->actionNameBytes.append(id);
    entry.name = d->actionNameBytes.last().data();
    entry.activate = [](GSimpleAction*, GVariant*, gpointer d) {
        GnomeMenuUserData* userData = static_cast<GnomeMenuUserData*>(d);
        QMetaObject::invokeMethod(userData->receiver, userData->slot);
    };

    d->entries.append(entry);
    d->entriesUserDatas.append(GnomeMenuUserData{receiver, slot});

    d->actionNameBytes.append("app." + id);
    g_menu_append(d->menu, caption.toUtf8().data(), d->actionNameBytes.last().data());
}

void GnomeMenu::addToggleItem(const QByteArray& id, const QString& caption, bool isChecked, QObject* receiver, const char* slot)
{
    GActionEntry entry;
    memset(&entry, 0, sizeof(GActionEntry));
    d->actionNameBytes.append(id);
    entry.name = d->actionNameBytes.last().data();
    entry.state = isChecked ? "true" : "false";
    entry.change_state = [](GSimpleAction*action, GVariant* value, gpointer d) {
        g_simple_action_set_state(action, value);
        GnomeMenuUserData* userData = static_cast<GnomeMenuUserData*>(d);
        QMetaObject::invokeMethod(userData->receiver, userData->slot);
    };

    d->entries.append(entry);
    d->entriesUserDatas.append(GnomeMenuUserData{receiver, slot});

    d->actionNameBytes.append("app." + id);
    g_menu_append(d->menu, caption.toUtf8().data(), d->actionNameBytes.last().data());
}

void GnomeMenu::setToggleItemState(const QByteArray& id, bool checked)
{
    for (int i = 0; i < d->entries.length(); ++i) {
        GActionEntry& entry = d->entries[i];
        if (qstrcmp(id.data(), entry.name) == 0) {
            entry.state = checked ? "true" : "false";
            if (d->group) {
                GAction* action = g_action_map_lookup_action(G_ACTION_MAP(d->group), entry.name);
                if (action)
                    g_simple_action_set_state(G_SIMPLE_ACTION(action), g_variant_new_boolean(checked));
            }
            break;
        }
    }
}

void GnomeMenu::addSection(GnomeMenu* section)
{
    section->setParent(this);
    d->entries.append(section->d->entries);
    d->entriesUserDatas.append(section->d->entriesUserDatas);

    g_menu_append_section(d->menu, nullptr, G_MENU_MODEL(section->d->menu));
}

void GnomeMenu::setMenuToWindow(WId winId, const QByteArray& dbusPathPrefix)
{
    QByteArray dbusActionPath = dbusPathPrefix + "/actions";
    QByteArray dbusMenuPath = dbusPathPrefix + "/menuitems";

    if (d->group)
        g_object_unref(G_OBJECT(d->group));
    d->group = g_simple_action_group_new();
    for (int i = 0; i < d->entries.length(); i++)
        g_action_map_add_action_entries(G_ACTION_MAP(d->group), &d->entries[i], 1, (gpointer)&d->entriesUserDatas[i]);

    GError* err = nullptr;

    if (d->exportedActionsId)
        g_dbus_connection_unexport_action_group(d->dbusSession, d->exportedActionsId);
    d->exportedActionsId = g_dbus_connection_export_action_group(d->dbusSession, dbusActionPath, G_ACTION_GROUP(d->group), &err);
    if (err)
        qWarning() << "g_dbus_connection_export_action_group:" << err->code << err->message;

    if (d->exportedMenuId)
        g_dbus_connection_unexport_menu_model(d->dbusSession, d->exportedMenuId);
    d->exportedMenuId = g_dbus_connection_export_menu_model(d->dbusSession, dbusMenuPath, G_MENU_MODEL(d->menu), &err);
    if (err)
        qWarning() << "g_dbus_connection_export_menu_model:" << err->code << err->message;

    X11Helper::setWindowProperty(winId, "_GTK_APPLICATION_ID", QCoreApplication::applicationName().toUtf8());
    X11Helper::setWindowProperty(winId, "_GTK_UNIQUE_BUS_NAME", g_dbus_connection_get_unique_name(d->dbusSession));
    X11Helper::setWindowProperty(winId, "_GTK_APPLICATION_OBJECT_PATH", dbusActionPath);
    X11Helper::setWindowProperty(winId, "_GTK_APP_MENU_OBJECT_PATH", dbusMenuPath);

    // NOTE: there is also a _GTK_WINDOW_OBJECT_PATH but it's not needed at the moment
}

#endif // COMMUNI_HAVE_GIO
