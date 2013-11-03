/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "textbrowserex.h"
#include "bufferview.h"
#include <QDesktopServices>
#include <QContextMenuEvent>
#include <QMenu>

TextBrowserEx::TextBrowserEx(QWidget* parent) : TextBrowser(parent)
{
    setOpenLinks(false);
    setTabChangesFocus(true);

    connect(this, SIGNAL(anchorClicked(QUrl)), this, SLOT(onAnchorClicked(QUrl)));
}

TextBrowserEx::~TextBrowserEx()
{
}

void TextBrowserEx::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = createStandardContextMenu(event->pos());
    menu->addSeparator();
    QAction* splitAction = menu->addAction(tr("Split"));
    QAction* splitSideAction = menu->addAction(tr("Split side by side"));
    QAction* closeAction = menu->addAction(tr("Close"));
    closeAction->setEnabled(window()->findChildren<BufferView*>().count() > 1);
    QAction* action = menu->exec(event->globalPos());
    if (action == splitAction)
        emit split(Qt::Vertical);
    else if (action == splitSideAction)
        emit split(Qt::Horizontal);
    else if (action == closeAction)
        parent()->parent()->deleteLater();
    delete menu;
}

void TextBrowserEx::onAnchorClicked(const QUrl& url)
{
    if (url.scheme() == "nick")
        emit clicked(url.toString(QUrl::RemoveScheme));
    else
        QDesktopServices::openUrl(url);
    clearFocus();
}
