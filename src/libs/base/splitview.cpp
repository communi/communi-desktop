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
    d.current = createBufferView(this);
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget*,QWidget*)));
}

IrcBuffer* SplitView::currentBuffer() const
{
    if (d.current)
        return d.current->buffer();
    return 0;
}

BufferView* SplitView::currentView() const
{
    return d.current;
}

QList<BufferView*> SplitView::views() const
{
    return d.views;
}

void SplitView::setCurrentView(BufferView *view)
{
    if (view)
        view->setFocus();
}

void SplitView::setCurrentBuffer(IrcBuffer* buffer)
{
    if (d.current)
        d.current->setBuffer(buffer);
}

void SplitView::split(Qt::Orientation orientation)
{
    if (d.current) {
        QSplitter* container = qobject_cast<QSplitter*>(d.current->parentWidget());
        if (container) {
            const int size = (orientation == Qt::Horizontal ? container->width() : container->height()) - container->handleWidth();
            if (container->count() == 1 || container->orientation() == orientation) {
                container->setOrientation(orientation);
                BufferView* view = createBufferView(container);
                view->setBuffer(d.current->buffer());
                QList<int> sizes;
                for (int i = 0; i < container->count(); ++i)
                    sizes += size / container->count();
                container->setSizes(sizes);
            } else {
                int index = container->indexOf(d.current);
                if (index != -1) {
                    QList<int> sizes = container->sizes();
                    QSplitter* splitter = new QSplitter(orientation, container);
                    splitter->setHandleWidth(1);
                    container->insertWidget(index, splitter);
                    splitter->addWidget(d.current);
                    container->setSizes(sizes);
                    BufferView* view = createBufferView(splitter);
                    view->setBuffer(d.current->buffer());
                    splitter->setSizes(QList<int>() << size/2 << size/2);
                }
            }
        }
    }
}

BufferView* SplitView::createBufferView(QSplitter* splitter)
{
    BufferView* view = new BufferView(splitter);
    connect(view, SIGNAL(destroyed(BufferView*)), this, SLOT(onViewRemoved(BufferView*)));
    d.views += view;
    splitter->addWidget(view);
    emit viewAdded(view);
    return view;
}

void SplitView::onViewRemoved(BufferView* view)
{
    d.views.removeOne(view);
    emit viewRemoved(view);
}

void SplitView::onFocusChanged(QWidget*, QWidget* widget)
{
    while (widget) {
        BufferView* view = qobject_cast<BufferView*>(widget);
        if (view && d.current != view) {
            d.current = view;
            emit currentViewChanged(view);
            emit currentBufferChanged(view->buffer());
            break;
        }
        widget = widget->parentWidget();
    }
}
