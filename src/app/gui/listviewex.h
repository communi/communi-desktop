/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef LISTVIEWEX_H
#define LISTVIEWEX_H

#include "listview.h"

class ListViewEx : public ListView
{
    Q_OBJECT

public:
    ListViewEx(QWidget* parent = 0);

signals:
    void doubleClicked(const QString& user);

protected:
    QSize sizeHint() const;

private slots:
    void onDoubleClicked(const QModelIndex& index);
};

#endif // LISTVIEWEX_H
