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

#include "searchpopup.h"
#include <QMouseEvent>
#include <QShortcut>

SearchPopup::SearchPopup(QTreeWidget* parent) : QLineEdit(parent)
{
    d.tree = parent;

    setWindowOpacity(0.8);
    setWindowFlags(Qt::Popup);
    setAttribute(Qt::WA_DeleteOnClose);
    setFont(QFont(font().family(), 36));

    QShortcut* shortcut = new QShortcut(QKeySequence("Esc"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

    connect(this, SIGNAL(textEdited(QString)), this, SLOT(search(QString)));
    connect(this, SIGNAL(returnPressed()), this, SLOT(searchAgain()));
}

SearchPopup::~SearchPopup()
{
}

void SearchPopup::popup()
{
    show();
    raise();
    setFocus();
    activateWindow();
}

void SearchPopup::mousePressEvent(QMouseEvent* event)
{
    QLineEdit::mousePressEvent(event);
    if (!geometry().contains(event->globalPos()))
        close();
}

void SearchPopup::search(const QString& txt)
{
    if (d.tree && !txt.isEmpty()) {
        QList<QTreeWidgetItem*> items = d.tree->findItems(txt, Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive);
        if (items.isEmpty())
            items = d.tree->findItems(txt, Qt::MatchContains | Qt::MatchWrap | Qt::MatchRecursive);
        if (!items.isEmpty() && !items.contains(d.tree->currentItem()))
            d.tree->setCurrentItem(items.first());
        setStyleSheet(!items.isEmpty() ? "" : "background: #ff7a7a");
    }
}

void SearchPopup::searchAgain()
{
    QString txt = text();
    if (d.tree && !txt.isEmpty()) {
        QTreeWidgetItem* item = d.tree->currentItem();
        if (item) {
            QTreeWidgetItemIterator it(item, QTreeWidgetItemIterator::Unselected);
            bool wrapped = false;
            while (*it) {
                if ((*it)->text(0).contains(txt, Qt::CaseInsensitive)) {
                    d.tree->setCurrentItem(*it);
                    return;
                }
                ++it;
                if (!(*it) && !wrapped) {
                    it = QTreeWidgetItemIterator(d.tree, QTreeWidgetItemIterator::Unselected);
                    wrapped = true;
                }
            }
        }
    }
}
