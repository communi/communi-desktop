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

#include "listfinder.h"
#include "listview.h"
#include <Irc>

ListFinder::ListFinder(ListView* list) : Finder(list)
{
    d.list = list;
    connect(this, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
}

void ListFinder::find(const QString& text, bool forward, bool backward, bool typed)
{
    Q_UNUSED(backward);
    if (!d.list || !d.list->model() || text.isEmpty())
        return;

    QAbstractItemModel* model = d.list->model();
    if (typed) {
        QList<QModelIndex> indexes = model->match(model->index(0, 0), Irc::NameRole, text, -1, Qt::MatchExactly | Qt::MatchWrap);
        if (indexes.isEmpty())
            indexes = model->match(model->index(0, 0), Irc::NameRole, text, -1, Qt::MatchContains | Qt::MatchWrap);
        if (!indexes.isEmpty() && !indexes.contains(d.list->currentIndex()))
            d.list->setCurrentIndex(indexes.first());
        setError(indexes.isEmpty());
    } else {
        QModelIndex index = d.list->currentIndex();
        if (index.isValid()) {
            if (forward)
                index = index.sibling(index.row() + 1, index.column());
            else
                index = index.sibling(index.row() - 1, index.column());
            bool wrapped = false;
            while (index.isValid()) {
                if (index.data(Irc::NameRole).toString().contains(text, Qt::CaseInsensitive)) {
                    d.list->setCurrentIndex(index);
                    return;
                }
                if (forward)
                    index = index.sibling(index.row() + 1, index.column());
                else
                    index = index.sibling(index.row() - 1, index.column());
                if (!index.isValid() && !wrapped) {
                    if (forward)
                        index = model->index(0, 0);
                    else
                        index = model->index(model->rowCount() - 1, 0);
                    wrapped = true;
                }
            }
        }
    }
}

void ListFinder::relocate()
{
    QRect r = rect();
    QRect br = parentWidget()->rect();
    r.moveBottom(br.bottom());
    r.setWidth(br.width() + 2);
    r.translate(-1, -offset());
    setGeometry(r);
}

void ListFinder::onReturnPressed()
{
    if (!d.list)
        return;

    QModelIndex index = d.list->currentIndex();
    if (index.isValid()) {
        const QString user = index.data(Irc::NameRole).toString();
        QMetaObject::invokeMethod(d.list, "queried", Q_ARG(QString, user));
        animateHide();
    }
}
