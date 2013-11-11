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

#ifndef LISTMENU_H
#define LISTMENU_H

#include <QMenu>
#include <QModelIndex>

class ListView;

class ListMenu : public QMenu
{
    Q_OBJECT

public:
    ListMenu(ListView* list);

    bool eventFilter(QObject *object, QEvent *event);

public slots:
    void exec(const QModelIndex& index, const QPoint& pos);

private slots:
    void onWhoisTriggered();
    void onQueryTriggered();
    void onModeTriggered();
    void onKickTriggered();
    void onBanTriggered();

private:
    void setup(const QModelIndex& index);

    struct Private {
        ListView* list;
        QAction* whoisAction;
        QAction* queryAction;
        QAction* opAction;
        QAction* voiceAction;
        QAction* kickAction;
        QAction* banAction;
    } d;
};

#endif // MENUPLUGIN_H
