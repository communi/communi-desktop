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

#ifndef WINDOWPLUGIN_H
#define WINDOWPLUGIN_H

#include <QtPlugin>
#include <QWidget>

class WindowPlugin : public QObject
{
public:
    virtual ~WindowPlugin() {}
    virtual void initWindow(QWidget*) {}
    virtual void cleanupWindow(QWidget*) {}

    QWidget* window() const;
    bool isActiveWindow() const;
    virtual void windowActivated() {}
    virtual void windowDeactivated() {}

protected:
    bool eventFilter(QObject* object, QEvent* event);

private:
    friend class PluginLoader;
    void filterWindow(QWidget* window);
};

Q_DECLARE_INTERFACE(WindowPlugin, "Communi.WindowPlugin")

#endif // WINDOWPLUGIN_H
