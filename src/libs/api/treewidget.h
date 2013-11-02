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

#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QTreeWidget>

class TreeItem;
class IrcBuffer;
class IrcConnection;

class TreeWidget : public QTreeWidget
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* currentBuffer READ currentBuffer WRITE setCurrentBuffer NOTIFY currentBufferChanged)

public:
    TreeWidget(QWidget* parent = 0);

    virtual IrcBuffer* currentBuffer() const = 0;
    virtual TreeItem* bufferItem(IrcBuffer* buffer) const = 0;

    virtual QList<IrcConnection*> connections() const = 0;

public slots:
    virtual void addBuffer(IrcBuffer* buffer) = 0;
    virtual void removeBuffer(IrcBuffer* buffer) = 0;
    virtual void setCurrentBuffer(IrcBuffer* buffer) = 0;

signals:
    void currentBufferChanged(IrcBuffer* buffer);
};

#endif // TREEWIDGET_H
