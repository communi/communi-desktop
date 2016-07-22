/*
  Copyright (C) 2008-2016 The Communi Project

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

#include "listfinder.h"
#include "listview.h"
#include <Irc>

ListFinder::ListFinder(ListView* list) : AbstractFinder(list)
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
    raise();
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
