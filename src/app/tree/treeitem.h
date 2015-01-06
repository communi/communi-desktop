/*
  Copyright (C) 2008-2015 The Communi Project

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

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QObject>
#include <QMetaType>
#include <QTreeWidgetItem>
#include <QVariantAnimation>

class IrcBuffer;
class TreeWidget;
class IrcLagTimer;
class IrcConnection;

class TreeItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* buffer READ buffer CONSTANT)
    Q_PROPERTY(IrcConnection* connection READ connection CONSTANT)

public:
    TreeItem(IrcBuffer* buffer, TreeItem* parent);
    TreeItem(IrcBuffer* buffer, TreeWidget* parent);
    ~TreeItem();

    IrcBuffer* buffer() const;
    IrcConnection* connection() const;

    TreeItem* parentItem() const;
    TreeWidget* treeWidget() const;

    QVariant data(int column, int role) const;
    void setData(int column, int role, const QVariant& value);

    bool operator<(const QTreeWidgetItem& other) const;

public slots:
    void refresh();

signals:
    void destroyed(TreeItem* item);

private slots:
    void updateIcon();
    void onStatusChanged();
    void onBufferDestroyed();

private:
    void init(IrcBuffer* buffer);

    struct Private {
        IrcBuffer* buffer;
        IrcLagTimer* timer;
        QVariantAnimation* anim;
    } d;
};

Q_DECLARE_METATYPE(TreeItem*)

#endif // TREEITEM_H
