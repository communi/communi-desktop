/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef TEXTBROWSEREX_H
#define TEXTBROWSEREX_H

#include "textbrowser.h"

class TextBrowserEx : public TextBrowser
{
    Q_OBJECT

public:
    TextBrowserEx(QWidget* parent = 0);
    ~TextBrowserEx();

signals:
    void clicked(const QString& user);
    void split(Qt::Orientation orientation);

protected:
    void contextMenuEvent(QContextMenuEvent* event);

private slots:
    void onAnchorClicked(const QUrl& url);
};

#endif // TEXTBROWSEREX_H
