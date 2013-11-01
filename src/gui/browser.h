/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef BROWSER_H
#define BROWSER_H

#include "textbrowser.h"

class IrcBuffer;

class Browser : public TextBrowser
{
    Q_OBJECT

public:
    Browser(QWidget* parent = 0);
    ~Browser();

signals:
    void clicked(const QString& user);
    void split(Qt::Orientation orientation);

protected:
    void contextMenuEvent(QContextMenuEvent* event);

private slots:
    void onAnchorClicked(const QUrl& url);

private:
    struct Private {
    } d;
};

#endif // BROWSER_H
