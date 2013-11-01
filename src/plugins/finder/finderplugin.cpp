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

#include "finderplugin.h"
#include "treewidget.h"
#include <QMouseEvent>
#include <QShortcut>

FinderPlugin::FinderPlugin(QObject* parent) : QLineEdit()
{
    d.tree = 0;
    Q_UNUSED(parent);

    setVisible(false);
    setWindowOpacity(0.8);
    setWindowFlags(Qt::Popup);
    setWindowModality(Qt::WindowModal);
    setFont(QFont(font().family(), 36));

    connect(this, SIGNAL(textEdited(QString)), this, SLOT(search(QString)));
    connect(this, SIGNAL(returnPressed()), this, SLOT(searchAgain()));
}

void FinderPlugin::setTree(TreeWidget* tree)
{
    d.tree = tree;
    setParent(d.tree);

    QShortcut* searchShortcut = new QShortcut(this);
    searchShortcut->setKey(QKeySequence(tr("Ctrl+S")));
    connect(searchShortcut, SIGNAL(activated()), this, SLOT(popup()));

    QShortcut* hideShortcut = new QShortcut(this);
    hideShortcut->setKey(QKeySequence(tr("Esc")));
    connect(hideShortcut, SIGNAL(activated()), this, SLOT(hide()));
}

void FinderPlugin::popup()
{
    show();
    raise();
    setFocus();
    activateWindow();
}

void FinderPlugin::mousePressEvent(QMouseEvent* event)
{
    QLineEdit::mousePressEvent(event);
    if (!geometry().contains(event->globalPos()))
        close();
}

void FinderPlugin::search(const QString& txt)
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

void FinderPlugin::searchAgain()
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
