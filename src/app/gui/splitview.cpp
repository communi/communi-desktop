/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "splitview.h"
#include "bufferview.h"
#include <QApplication>

SplitView::SplitView(QWidget* parent) : QSplitter(parent)
{
    setHandleWidth(1);
    BufferView* view = new BufferView(this);
    connect(view, SIGNAL(split(Qt::Orientation)), this, SLOT(split(Qt::Orientation)));
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget*,QWidget*)));
    d.current = view;
    d.views += view;
    addWidget(view);
}

SplitView::~SplitView()
{
}

IrcBuffer* SplitView::currentBuffer() const
{
    return d.current->buffer();
}

void SplitView::setCurrentBuffer(IrcBuffer* buffer)
{
    if (d.current)
        d.current->setBuffer(buffer);
}

void SplitView::split(Qt::Orientation orientation)
{
    BufferView* current = qobject_cast<BufferView*>(sender());
    if (current) {
        QSplitter* container = qobject_cast<QSplitter*>(current->parentWidget());
        if (container) {
            const int size = (orientation == Qt::Horizontal ? container->width() : container->height()) - container->handleWidth();
            if (container->count() == 1 || container->orientation() == orientation) {
                container->setOrientation(orientation);
                BufferView* view = new BufferView(container);
                view->setBuffer(current->buffer());
                connect(view, SIGNAL(split(Qt::Orientation)), this, SLOT(split(Qt::Orientation)));
                container->addWidget(view);
                QList<int> sizes;
                for (int i = 0; i < container->count(); ++i)
                    sizes += size / container->count();
                container->setSizes(sizes);
            } else {
                int index = container->indexOf(current);
                if (index != -1) {
                    QList<int> sizes = container->sizes();
                    QSplitter* splitter = new QSplitter(orientation, container);
                    splitter->setHandleWidth(1);
                    container->insertWidget(index, splitter);
                    splitter->addWidget(current);
                    container->setSizes(sizes);
                    BufferView* view = new BufferView(splitter);
                    view->setBuffer(current->buffer());
                    connect(view, SIGNAL(split(Qt::Orientation)), this, SLOT(split(Qt::Orientation)));
                    splitter->addWidget(view);
                    splitter->setSizes(QList<int>() << size/2 << size/2);
                }
            }
        }
    }
}

void SplitView::onFocusChanged(QWidget*, QWidget* widget)
{
    while (widget) {
        BufferView* view = qobject_cast<BufferView*>(widget);
        if (view && d.current != view) {
            d.current = view;
            emit currentBufferChanged(view->buffer());
            break;
        }
        widget = widget->parentWidget();
    }
}
