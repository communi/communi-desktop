/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "splitview.h"
#include "textinput.h"
#include "bufferview.h"
#include <QApplication>
#include <QShortcut>
#include <IrcBuffer>

SplitView::SplitView(QWidget* parent) : QSplitter(parent)
{
    d.current = createBufferView(this);
    connect(d.current, SIGNAL(bufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget*,QWidget*)));

#ifdef Q_OS_MAC
    QString next = tr("Alt+Tab");
    QString previous = tr("Shift+Alt+Tab");
#else
    QString next = tr("Ctrl+Tab");
    QString previous = tr("Shift+Ctrl+Tab");
#endif

    QShortcut* shortcut = new QShortcut(QKeySequence(next), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(activateNextView()));

    shortcut = new QShortcut(QKeySequence(previous), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(activatePreviousView()));
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
    if (view && view->textInput())
        view->textInput()->setFocus();
}

void SplitView::setCurrentBuffer(IrcBuffer* buffer)
{
    if (d.current) {
        if (buffer)
            connect(buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(onBufferRemoved(IrcBuffer*)), Qt::UniqueConnection);
        d.current->setBuffer(buffer);
    }
}

QByteArray SplitView::saveState() const
{
    QVariantMap state;
    state.insert("views", saveSplittedViews(this));

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << state;
    return data;
}

void SplitView::restoreState(const QByteArray& data)
{
    QVariantMap state;
    QDataStream in(data);
    in >> state;

    if (state.contains("views"))
        restoreSplittedViews(this, state.value("views").toMap());
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
                QSplitter* splitter = wrap(view, orientation);
                if (splitter) {
                    container->setSizes(sizes);
                    bv = createBufferView(splitter);
                    splitter->setSizes(QList<int>() << size/2 << size/2);
                }
            }
            if (bv) {
                bv->setBuffer(view->buffer());
                emit viewAdded(bv);
            }
        }
    }
}

QSplitter* SplitView::wrap(BufferView* view, Qt::Orientation orientation)
{
    QSplitter* container = qobject_cast<QSplitter*>(view->parentWidget());
    if (container) {
        int index = container->indexOf(view);
        QSplitter* splitter = new QSplitter(orientation, container);
        container->insertWidget(index, splitter);
        container->setCollapsible(index, false);
        splitter->addWidget(view);
        splitter->setCollapsible(0, false);
        return splitter;
    }
    return 0;
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

void SplitView::activateNextView()
{
    if (d.views.count() > 1) {
        int index = d.views.indexOf(d.current) + 1;
        setCurrentView(d.views.value(index % d.views.count()));
    }
}

void SplitView::activatePreviousView()
{
    if (d.views.count() > 1) {
        int index = d.views.indexOf(d.current) - 1;
        if (index < 0)
            index = d.views.count() - 1;
        setCurrentView(d.views.value(index));
    }
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

void SplitView::onBufferAdded(IrcBuffer* buffer)
{
    // TODO: optimize
//    QList<BufferView*> views = findChildren<BufferView*>("__unrestored__");
//    foreach (BufferView* bv, views) {
//        if (bv->property("__buffer__").toString() == buffer->title()) {
//            TreeItem* item = d.tree->bufferItem(buffer);
//            TreeItem* parent = item ? item->parentItem() : 0;
//            if (bv->property("__parent__").toString() == (parent ? parent->text(0) : QString())) {
//                if (bv->property("__index__").toInt() == d.tree->indexOfTopLevelItem(parent ? parent : item)) {
//                    bv->setBuffer(buffer);
//                    bv->setObjectName(QString());
//                    bv->setProperty("__parent__", QVariant());
//                    bv->setProperty("__buffer__", QVariant());
//                    bv->setProperty("__index__", QVariant());
//                }
//            }
//        }
//    }
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

QVariantMap SplitView::saveSplittedViews(const QSplitter* splitter) const
{
    QVariantMap state;
    state.insert("count", splitter->count());
    if (QSplitter* parent = qobject_cast<QSplitter*>(splitter->parentWidget()))
        state.insert("index", parent->indexOf(const_cast<QSplitter*>(splitter)));
    state.insert("state", splitter->saveState());
    state.insert("geometry", splitter->saveGeometry());
    state.insert("orientation", splitter->orientation());
    QVariantList buffers;
    QVariantList children;
    for (int i = 0; i < splitter->count(); ++i) {
        QSplitter* child = qobject_cast<QSplitter*>(splitter->widget(i));
        if (child)
            children += saveSplittedViews(child);
        BufferView* bv = qobject_cast<BufferView*>(splitter->widget(i));
        if (bv) {
//            TreeItem* item = d.tree->bufferItem(bv->buffer());
//            TreeItem* parent = item ? item->parentItem() : 0;
            QVariantMap buffer;
//            buffer.insert("parent", parent ? parent->text(0) : QString());
//            buffer.insert("buffer", item ? item->text(0) : QString());
//            buffer.insert("index", d.tree->indexOfTopLevelItem(parent ? parent : item));
            if (QSplitter* sp = bv->findChild<QSplitter*>())
                buffer.insert("state", sp->saveState());
            buffers += buffer;
        }
    }
    state.insert("buffers", buffers);
    state.insert("children", children);
    return state;
}

void SplitView::restoreSplittedViews(QSplitter* splitter, const QVariantMap& state)
{
    int count = state.value("count", -1).toInt();
    if (count > 1) {
        BufferView* bv = qobject_cast<BufferView*>(splitter->widget(0));
        Q_ASSERT(bv);
        Qt::Orientation orientation = static_cast<Qt::Orientation>(state.value("orientation").toInt());
        for (int i = 1; i < count; ++i)
            split(bv, orientation);

        QVariantList children = state.value("children").toList();
        foreach (const QVariant& v, children) {
            QVariantMap child = v.toMap();
            int index = child.value("index", -1).toInt();
            Qt::Orientation ori = static_cast<Qt::Orientation>(child.value("orientation").toInt());
            BufferView* bv = qobject_cast<BufferView*>(splitter->widget(index));
            if (bv) {
                QSplitter* parent = wrap(bv, ori);
                if (parent)
                    restoreSplittedViews(parent, child);
            }
        }
    }

    QVariantList buffers = state.value("buffers").toList();
    for (int i = 0; !buffers.isEmpty() && i < splitter->count(); ++i) {
        BufferView* bv = qobject_cast<BufferView*>(splitter->widget(i));
        if (bv) {
            QVariantMap buffer = buffers.takeFirst().toMap();
            bv->setProperty("__parent__", buffer.value("parent").toString());
            bv->setProperty("__buffer__", buffer.value("buffer").toString());
            bv->setProperty("__index__", buffer.value("index").toInt());
            if (buffer.contains("state")) {
                if (QSplitter* sp = bv->findChild<QSplitter*>())
                    sp->restoreState(buffer.value("state").toByteArray());
            }
            bv->setObjectName("__unrestored__");
        }
    }

    if (state.contains("geometry"))
        splitter->restoreGeometry(state.value("geometry").toByteArray());
    if (state.contains("state"))
        splitter->restoreState(state.value("state").toByteArray());
}
