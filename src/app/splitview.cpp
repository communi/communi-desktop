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

#include "splitview.h"
#include "textinput.h"
#include "titlebar.h"
#include "bufferview.h"
#include "textbrowser.h"
#include <QContextMenuEvent>
#include <IrcConnection>
#include <QApplication>
#include <QScrollBar>
#include <QShortcut>
#include <IrcBuffer>
#include <QAction>
#include <QMenu>

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

    connect(this, SIGNAL(viewAdded(BufferView*)), this, SLOT(updateActions()));
    connect(this, SIGNAL(viewRemoved(BufferView*)), this, SLOT(updateActions()));
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
    BufferView* view = currentView();
    if (view)
        view->setBuffer(buffer);
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
    split(currentView(), orientation);
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

void SplitView::addBuffer(IrcBuffer* buffer)
{
    IrcConnection* connection = buffer->connection();
    if (connection) {
        const QString uuid = connection->userData().value("uuid").toString();
        if (!uuid.isEmpty()) {
            // TODO: optimize
            QList<BufferView*> views = findChildren<BufferView*>("__unrestored__");
            foreach (BufferView* bv, views) {
                if (bv->property("__uuid__").toString() == uuid &&
                        bv->property("__buffer__").toString() == buffer->title()) {
                    bv->setBuffer(buffer);
                    bv->setObjectName(QString());
                    bv->setProperty("__buffer__", QVariant());
                    bv->setProperty("__uuid__", QVariant());
                }
            }
        }
    }
}

void SplitView::removeBuffer(IrcBuffer* buffer)
{
    foreach (BufferView* view, d.views) {
        if (view->buffer() == buffer)
            view->setBuffer(currentBuffer());
    }
}

BufferView* SplitView::createBufferView(QSplitter* splitter, int index)
{
    BufferView* view = new BufferView(splitter);
    connect(view, SIGNAL(destroyed(BufferView*)), this, SLOT(onViewRemoved(BufferView*)));
    connect(view->textBrowser(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    // TODO: because of theme preview
    if (window()->inherits("QMainWindow")) {
        QMenu* menu = view->titleBar()->menu();
        addViewActions(menu, view);
        menu->addSeparator();
        addZoomActions(menu, view);
        menu->addSeparator();
        addSplitActions(menu, view);
        menu->addSeparator();
        addGlobalActions(menu);
    }

    d.views += view;
    splitter->insertWidget(index, view);
    splitter->setCollapsible(splitter->indexOf(view), false);
    return view;
}

void SplitView::activateNextView()
{
    if (d.views.count() > 1) {
        int index = d.views.indexOf(currentView()) + 1;
        setCurrentView(d.views.value(index % d.views.count()));
    }
}

void SplitView::activatePreviousView()
{
    if (d.views.count() > 1) {
        int index = d.views.indexOf(currentView()) - 1;
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
        if (view == currentView())
            setCurrentView(d.views.value(qMax(0, index - 1)));
    }
}

void SplitView::onFocusChanged(QWidget*, QWidget* widget)
{
    while (widget) {
        BufferView* view = qobject_cast<BufferView*>(widget);
        if (view && d.current != view) {
            BufferView* previous = d.current;
            if (previous)
                disconnect(previous, SIGNAL(bufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
            d.current = view;
            if (view)
                connect(view, SIGNAL(bufferChanged(IrcBuffer*)), this, SIGNAL(currentBufferChanged(IrcBuffer*)));
            emit currentViewChanged(view, previous);
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
            QVariantMap buf;
            IrcBuffer* buffer = bv->buffer();
            IrcConnection* connection = buffer ? buffer->connection() : 0;
            buf.insert("buffer", buffer ? buffer->title() : QString());
            buf.insert("current", bv == d.current);
            buf.insert("uuid", connection ? connection->userData().value("uuid").toString() : QString());
            if (QSplitter* sp = bv->findChild<QSplitter*>())
                buf.insert("state", sp->saveState());
            buf.insert("fontSize", bv->textBrowser()->font().pointSize());
            buf.insert("fontFamily", bv->textBrowser()->font().family());
            buffers += buf;
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
            QVariantMap buf = buffers.takeFirst().toMap();
            bv->setProperty("__buffer__", buf.value("buffer").toString());
            bv->setProperty("__uuid__", buf.value("uuid").toString());
            if (buf.contains("state")) {
                if (QSplitter* sp = bv->findChild<QSplitter*>())
                    sp->restoreState(buf.value("state").toByteArray());
            }
            QFont f = font();
            if (buf.contains("fontFamily"))
                f.setFamily(buf.value("fontFamily").toString());
            if (buf.contains("fontSize"))
                f.setPointSize(buf.value("fontSize").toInt());
            bv->textBrowser()->setFont(f);
            if (buf.value("current", false).toBool())
                setCurrentView(bv);
            bv->setObjectName("__unrestored__");
        }
    }

    if (state.contains("geometry"))
        splitter->restoreGeometry(state.value("geometry").toByteArray());
    if (state.contains("state"))
        splitter->restoreState(state.value("state").toByteArray());
}

void SplitView::updateActions()
{
    QMutableListIterator<QPointer<QAction> > it(d.unsplitters);
    while (it.hasNext()) {
        QPointer<QAction> action = it.next();
        if (action.isNull())
            it.remove();
        else
            action->setEnabled(d.views.count() > 1);
    }
}

void SplitView::splitVertical()
{
    BufferView* view = targetView();
    if (view)
        split(view, Qt::Vertical);
}

void SplitView::splitHorizontal()
{
    BufferView* view = targetView();
    if (view)
        split(view, Qt::Horizontal);
}

void SplitView::unsplit()
{
    BufferView* view = targetView();
    if (view)
        view->deleteLater();
}

void SplitView::zoomIn()
{
    BufferView* view = targetView();
    if (view)
        view->textBrowser()->zoomIn();
}

void SplitView::zoomOut()
{
    BufferView* view = targetView();
    if (view)
        view->textBrowser()->zoomOut();
}

void SplitView::resetZoom()
{
    BufferView* view = targetView();
    if (view)
        view->textBrowser()->resetZoom();
}

void SplitView::closeView()
{
    BufferView* view = targetView();
    if (view)
        view->closeBuffer();
}

void SplitView::joinChannel()
{
    BufferView* view = targetView();
    if (view) {
        view->textInput()->setText("/JOIN #");
        view->textInput()->setFocus();
    }
}

void SplitView::openQuery()
{
    BufferView* view = targetView();
    if (view) {
        view->textInput()->setText("/QUERY ");
        view->textInput()->setFocus();
    }
}

BufferView* SplitView::targetView() const
{
    BufferView* view = 0;
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
        view = action->data().value<BufferView*>();
    if (!view)
        view = currentView();
    return view;
}

void SplitView::addViewActions(QMenu* menu, BufferView* view)
{
    QAction* connectAction = menu->addAction(tr("Connect..."), window(), SLOT(doConnect()), QKeySequence::New);
    connectAction->setShortcutContext(Qt::WidgetShortcut);

    QAction* joinAction = menu->addAction(tr("Join channel"), this, SLOT(joinChannel()));
    joinAction->setData(QVariant::fromValue(view));

    QAction* queryAction = menu->addAction(tr("Open query"), this, SLOT(openQuery()));
    queryAction->setData(QVariant::fromValue(view));

    QAction* closeAction = menu->addAction(tr("Close"), this, SLOT(closeView()), QKeySequence::Close);
    closeAction->setData(QVariant::fromValue(view));
    closeAction->setShortcutContext(Qt::WidgetShortcut);
}

void SplitView::addSplitActions(QMenu* menu, BufferView* view)
{
    QAction* splitVAction = menu->addAction(tr("Split"), this, SLOT(splitVertical()));
    splitVAction->setData(QVariant::fromValue(view));

    QAction* splitHAction = menu->addAction(tr("Split side by side"), this, SLOT(splitHorizontal()));
    splitHAction->setData(QVariant::fromValue(view));

    QAction* unsplitAction = menu->addAction(tr("Unsplit"), this, SLOT(unsplit()));
    unsplitAction->setData(QVariant::fromValue(view));

    d.unsplitters.append(unsplitAction);
    updateActions();
}

void SplitView::addZoomActions(QMenu* menu, BufferView* view)
{
    QAction* zoomInAction = menu->addAction(tr("Zoom in"), this, SLOT(zoomIn()), QKeySequence::ZoomIn);
    zoomInAction->setShortcutContext(Qt::WidgetShortcut);
    zoomInAction->setData(QVariant::fromValue(view));

    QAction* zoomOutAction = menu->addAction(tr("Zoom out"), this, SLOT(zoomOut()), QKeySequence::ZoomOut);
    zoomOutAction->setShortcutContext(Qt::WidgetShortcut);
    zoomOutAction->setData(QVariant::fromValue(view));

    QAction* resetZoomAction = menu->addAction(tr("Reset zoom"), this, SLOT(resetZoom()), QKeySequence(tr("Ctrl+0")));
    resetZoomAction->setEnabled(view->textBrowser()->isZoomed());
    resetZoomAction->setShortcutContext(Qt::WidgetShortcut);
    resetZoomAction->setData(QVariant::fromValue(view));
}

void SplitView::addGlobalActions(QMenu* menu)
{
    QAction* settingsAction = menu->addAction(tr("Settings..."), window(), SLOT(showSettings()), QKeySequence::Preferences);
    settingsAction->setShortcutContext(Qt::WidgetShortcut);

    QAction* helpAction = menu->addAction(tr("Help..."), window(), SLOT(showHelp()), QKeySequence::HelpContents);
    helpAction->setShortcutContext(Qt::WidgetShortcut);

    menu->addSeparator();

    QAction* quitAction = menu->addAction(tr("Quit"), window(), SLOT(close()), QKeySequence::Quit);
    quitAction->setShortcutContext(Qt::WidgetShortcut);
}

void SplitView::showContextMenu(const QPoint& pos)
{
    TextBrowser* browser = qobject_cast<TextBrowser*>(sender());
    if (browser) {
        // select nick under to enable "Copy"
        const QString anchor = browser->anchorAt(pos);
        if (anchor.startsWith("nick:")) {
            QTextCursor cursor = browser->cursorForPosition(pos);
            cursor.select(QTextCursor::WordUnderCursor);
            browser->setTextCursor(cursor);
        }

        QMenu* menu = browser->createContextMenu(pos);

        QAction* restoreUsers = 0;
        QAction* restoreViews = 0;

        QSplitter* splitter = qobject_cast<QSplitter*>(browser->parentWidget());
        if (splitter && anchor.isEmpty()) {
            BufferView* view = qobject_cast<BufferView*>(splitter->parentWidget());
            if (view) {
                menu->addSeparator();
                addZoomActions(menu, view);
                menu->addSeparator();
                addSplitActions(menu, view);
            }

            QAction* separator = 0;

            IrcBuffer* buffer = browser->buffer();
            if (buffer && buffer->isChannel()) {
                if (splitter && splitter->sizes().value(1) == 0) {
                    separator = menu->addSeparator();
                    restoreUsers = menu->addAction(tr("Restore users"));
                }
            }

            QSplitter* splitter = window()->findChild<QSplitter*>();
            if (splitter && splitter->sizes().value(0) == 0) {
                if (!separator)
                    menu->addSeparator();
                restoreViews = menu->addAction(tr("Restore views"));
            }
        }

        QAction* action = menu->exec(browser->viewport()->mapToGlobal(pos));

        QSplitter* restoreSplitter = 0;
        if (restoreUsers && action == restoreUsers)
            restoreSplitter = qobject_cast<QSplitter*>(browser->parentWidget());
        else if (restoreViews && action == restoreViews)
            restoreSplitter = window()->findChild<QSplitter*>();
        if (restoreSplitter)
            restoreSplitter->setSizes(QList<int>() << restoreSplitter->widget(0)->sizeHint().width() << restoreSplitter->widget(1)->sizeHint().width());

        menu->deleteLater();

        // clear automatically done nick selection
        if (anchor.startsWith("nick:")) {
            QTextCursor cursor = browser->textCursor();
            cursor.clearSelection();
            browser->setTextCursor(cursor);
        }
    }
}
