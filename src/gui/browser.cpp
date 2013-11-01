/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "browser.h"
#include <QDesktopServices>
#include <QContextMenuEvent>
#include <QMenu>

Browser::Browser(QWidget* parent) : TextBrowser(parent)
{
    setOpenLinks(false);
    setTabChangesFocus(true);

    connect(this, SIGNAL(anchorClicked(QUrl)), this, SLOT(onAnchorClicked(QUrl)));
}

Browser::~Browser()
{
}

void Browser::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = createStandardContextMenu(event->pos());
    menu->addSeparator();
    QAction* splitAction = menu->addAction(tr("Split"));
    QAction* splitSideAction = menu->addAction(tr("Split side by side"));
    QAction* closeAction = menu->addAction(tr("Close"));
    QAction* action = menu->exec(event->globalPos());
    if (action == splitAction)
        emit split(Qt::Vertical);
    else if (action == splitSideAction)
        emit split(Qt::Horizontal);
    else if (action == closeAction)
        deleteLater();
    delete menu;
}

void Browser::onAnchorClicked(const QUrl& url)
{
    if (url.scheme() == "nick")
        emit clicked(url.toString(QUrl::RemoveScheme));
    else
        QDesktopServices::openUrl(url);
    clearFocus();
}
