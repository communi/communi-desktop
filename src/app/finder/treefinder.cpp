/*
  Copyright (C) 2008-2014 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "treefinder.h"
#include "treewidget.h"

TreeFinder::TreeFinder(TreeWidget* tree) : AbstractFinder(tree)
{
    d.tree = tree;
    if (tree)
        tree->blockItemReset(true);
    connect(this, SIGNAL(returnPressed()), this, SLOT(animateHide()));
}

void TreeFinder::setVisible(bool visible)
{
    AbstractFinder::setVisible(visible);
    if (!visible && d.tree)
        d.tree->blockItemReset(false);
}

void TreeFinder::find(const QString& text, bool forward, bool backward, bool typed)
{
    Q_UNUSED(backward);
    if (!d.tree || text.isEmpty())
        return;

    if (typed) {
        QList<QTreeWidgetItem*> items = d.tree->findItems(text, Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive);
        if (items.isEmpty())
            items = d.tree->findItems(text, Qt::MatchContains | Qt::MatchWrap | Qt::MatchRecursive);
        if (!items.isEmpty() && !items.contains(d.tree->currentItem()))
            d.tree->setCurrentItem(items.first());
        setError(items.isEmpty());
    } else {
        QTreeWidgetItem* item = d.tree->currentItem();
        if (item) {
            QTreeWidgetItemIterator it(item);
            if (forward)
                ++it;
            else
                --it;
            bool wrapped = false;
            while (*it) {
                if ((*it)->text(0).contains(text, Qt::CaseInsensitive)) {
                    d.tree->setCurrentItem(*it);
                    return;
                }
                if (forward)
                    ++it;
                else
                    --it;
                if (!(*it) && !wrapped) {
                    if (forward)
                        it = QTreeWidgetItemIterator(d.tree);
                    else
                        it = QTreeWidgetItemIterator(lastItem());
                    wrapped = true;
                }
            }
        }
    }
}

void TreeFinder::relocate()
{
    QRect r = rect();
    QRect br = parentWidget()->rect();
    r.moveBottom(br.bottom());
    r.setWidth(br.width() + 2);
    r.translate(-1, -offset());
    setGeometry(r);
    raise();
}

QTreeWidgetItem* TreeFinder::lastItem() const
{
    QTreeWidgetItem* item = d.tree->topLevelItem(d.tree->topLevelItemCount() - 1);
    if (item && item->childCount() > 0)
        item = item->child(item->childCount() - 1);
    return item;
}
