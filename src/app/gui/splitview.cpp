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
#include "commandparser.h"
#include "textbrowser.h"
#include "topiclabel.h"
#include "textinput.h"
#include "listview.h"
#include <QApplication>

// TODO:
#include <QPluginLoader>
#include "browserplugin.h"
#include "inputplugin.h"
#include "listplugin.h"
#include "topicplugin.h"

// TODO:
Q_IMPORT_PLUGIN(CompleterPlugin)
//Q_IMPORT_PLUGIN(FinderPlugin)
Q_IMPORT_PLUGIN(HistoryPlugin)
//Q_IMPORT_PLUGIN(MenuPlugin)
Q_IMPORT_PLUGIN(SubjectPlugin)

SplitView::SplitView(QWidget* parent) : QSplitter(parent)
{
    setHandleWidth(1);
    BufferView* view = createBufferView(this);
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
                BufferView* view = createBufferView(container);
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
                    BufferView* view = createBufferView(splitter);
                    view->setBuffer(current->buffer());
                    splitter->addWidget(view);
                    splitter->setSizes(QList<int>() << size/2 << size/2);
                }
            }
        }
    }
}

BufferView* SplitView::createBufferView(QWidget* parent) const
{
    BufferView* view = new BufferView(parent);
    // TODO: connect(view, SIGNAL(split(Qt::Orientation)), this, SLOT(split(Qt::Orientation)));

    CommandParser* parser = new CommandParser(view);
    parser->setTriggers(QStringList("/"));
    parser->setTolerant(true);
    view->textInput()->setParser(parser);

    // TODO: move outta here...?
    foreach (QObject* instance, QPluginLoader::staticInstances()) {
        BrowserPlugin* browserPlugin = qobject_cast<BrowserPlugin*>(instance);
        if (browserPlugin)
            browserPlugin->initialize(view->textBrowser());
        InputPlugin* inputPlugin = qobject_cast<InputPlugin*>(instance);
        if (inputPlugin)
            inputPlugin->initialize(view->textInput());
        ListPlugin* listPlugin = qobject_cast<ListPlugin*>(instance);
        if (listPlugin)
            listPlugin->initialize(view->listView());
        TopicPlugin* topicPlugin = qobject_cast<TopicPlugin*>(instance);
        if (topicPlugin)
            topicPlugin->initialize(view->topicLabel());
    }

    return view;
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
