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
#include <IrcBuffer>

SplitView::SplitView(QWidget* parent) : QSplitter(parent)
{
    d.current = createBufferView(this);
    connect(d.current, SIGNAL(bufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
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
    if (d.current) {
        if (buffer)
            connect(buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(onBufferRemoved(IrcBuffer*)), Qt::UniqueConnection);
        d.current->setBuffer(buffer);
    }
}

void SplitView::reset()
{
    qDeleteAll(d.views);
    d.views.clear();
    d.current = createBufferView(this);
    emit viewAdded(d.current);
}

void SplitView::split(Qt::Orientation orientation)
{
    split(d.current, orientation);
}

void SplitView::split(BufferView* view, Qt::Orientation orientation)
{
    if (view) {
        QSplitter* container = qobject_cast<QSplitter*>(view->parentWidget());
        if (container) {
            int index = container->indexOf(view);
            BufferView* bv = 0;
            const int size = (orientation == Qt::Horizontal ? container->width() : container->height()) - container->handleWidth();
            if (container->count() == 1 || container->orientation() == orientation) {
                container->setOrientation(orientation);
                bv = createBufferView(container, index + 1);
                QList<int> sizes;
                for (int i = 0; i < container->count(); ++i)
                    sizes += size / container->count();
                container->setSizes(sizes);
            } else if (index != -1) {
                QList<int> sizes = container->sizes();
                QSplitter* splitter = new QSplitter(orientation, container);
                container->insertWidget(index, splitter);
                container->setCollapsible(index, false);
                splitter->addWidget(view);
                splitter->setCollapsible(0, false);
                container->setSizes(sizes);
                bv = createBufferView(splitter);
                splitter->setSizes(QList<int>() << size/2 << size/2);
            }
            if (bv) {
                bv->setBuffer(view->buffer());
                emit viewAdded(bv);
            }
        }
    }
}

BufferView* SplitView::createBufferView(QSplitter* splitter, int index)
{
    BufferView* view = new BufferView(splitter);
    connect(view, SIGNAL(destroyed(BufferView*)), this, SLOT(onViewRemoved(BufferView*)));
    d.views += view;
    splitter->insertWidget(index, view);
    splitter->setCollapsible(splitter->indexOf(view), false);
    return view;
}

void SplitView::onViewRemoved(BufferView* view)
{
    int index = d.views.indexOf(view);
    if (index != -1) {
        d.views.removeAt(index);
        emit viewRemoved(view);
        QSplitter* splitter = qobject_cast<QSplitter*>(view->parentWidget());
        if (splitter && splitter != this && splitter->count() == 1 && splitter->widget(0) == view)
            splitter->deleteLater();
        if (d.current == view)
            setCurrentView(d.views.value(qMax(0, index - 1)));
    }
}

void SplitView::onBufferRemoved(IrcBuffer* buffer)
{
    foreach (BufferView* view, d.views) {
        if (view->buffer() == buffer)
            view->setBuffer(d.current->buffer());
    }
}

void SplitView::onFocusChanged(QWidget*, QWidget* widget)
{
    while (widget) {
        BufferView* view = qobject_cast<BufferView*>(widget);
        if (view && d.current != view) {
            if (d.current)
                disconnect(d.current, SIGNAL(bufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
            d.current = view;
            if (d.current)
                connect(d.current, SIGNAL(bufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
            emit currentViewChanged(view);
            emit currentBufferChanged(view->buffer());
            break;
        }
        widget = widget->parentWidget();
    }
}
