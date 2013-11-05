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

#ifndef BADGEPLUGIN_H
#define BADGEPLUGIN_H

#include <QtPlugin>
#include "treeplugin.h"

class IrcBuffer;
class IrcMessage;

class BadgePlugin : public QObject, public TreePlugin
{
    Q_OBJECT
    Q_INTERFACES(TreePlugin)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.github.communi.TreePlugin")
#endif

public:
    BadgePlugin(QObject* parent = 0);

    void initialize(TreeWidget* tree);

private slots:
    void onBufferAdded(IrcBuffer* buffer);
    void onBufferRemoved(IrcBuffer* buffer);
    void onMessageReceived(IrcMessage* message);

private:
    struct Private {
        TreeWidget* tree;
    } d;
};

#endif // BADGEPLUGIN_H
