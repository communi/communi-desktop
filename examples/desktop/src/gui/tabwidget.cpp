/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
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

#include "tabwidget.h"
#include "tabwidget_p.h"
#include "sharedtimer.h"
#include <QContextMenuEvent>
#include <QStackedWidget>
#include <QStyleOption>
#include <QProxyStyle>
#include <QShortcut>

class TabWidgetStyle : public QProxyStyle
{
public:
    TabWidgetStyle(QStyle* style) : QProxyStyle(style) { }

    QRect subElementRect(SubElement se, const QStyleOption* opt, const QWidget* widget = 0) const
    {
        if (se == QStyle::SE_TabWidgetTabContents)
        {
            const TabWidget* tabWidget = qobject_cast<const TabWidget*>(widget);
            if (tabWidget && tabWidget->tabBar()->testAttribute(Qt::WA_WState_Hidden))
                return opt->rect;
        }
        return baseStyle()->subElementRect(se, opt, widget);
    }
};

TabBar::TabBar(QWidget* parent) : QTabBar(parent)
{
    prevShortcut = new QShortcut(this);
    connect(prevShortcut, SIGNAL(activated()), parent, SLOT(moveToPrevTab()));

    nextShortcut = new QShortcut(this);
    connect(nextShortcut, SIGNAL(activated()), parent, SLOT(moveToNextTab()));

    addTab(tr("+"));
    setSelectionBehaviorOnRemove(SelectLeftTab);
}

QKeySequence TabBar::navigationShortcut(Navigation navigation) const
{
    if (navigation == Previous)
        return prevShortcut->key();
    else
        return nextShortcut->key();
}

void TabBar::setNavigationShortcut(Navigation navigation, const QKeySequence& shortcut)
{
    if (navigation == Previous)
        prevShortcut->setKey(shortcut);
    else
        nextShortcut->setKey(shortcut);
}

QSize TabBar::minimumSizeHint() const
{
    return testAttribute(Qt::WA_WState_Hidden) ? QSize() : QTabBar::minimumSizeHint();
}

void TabBar::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::StyleChange)
    {
        Qt::TextElideMode mode = elideMode();
        QTabBar::changeEvent(event);
        if (mode != elideMode())
            setElideMode(mode);
        return;
    }
    QTabBar::changeEvent(event);
}

void TabBar::contextMenuEvent(QContextMenuEvent* event)
{
    int index = tabAt(event->pos());
    if (index != -1)
        emit menuRequested(index, event->globalPos());
}

void TabBar::wheelEvent(QWheelEvent* event)
{
    if (event->delta() > 0)
        QMetaObject::invokeMethod(parent(), "moveToPrevTab");
    else
        QMetaObject::invokeMethod(parent(), "moveToNextTab");
    QWidget::wheelEvent(event);
}

TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent)
{
    setTabBar(new TabBar(this));
    setElideMode(Qt::ElideMiddle);
    setStyle(new TabWidgetStyle(style()));
    d.previous = -1;
    d.updatingColors = false;

    d.colors[Active] = palette().color(QPalette::WindowText);
    d.colors[Inactive] = palette().color(QPalette::Disabled, QPalette::Highlight);
    d.colors[Alert] = palette().color(QPalette::Highlight);
    d.colors[Highlight] = palette().color(QPalette::Highlight);

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(tabBar(), SIGNAL(menuRequested(int,QPoint)), this, SIGNAL(tabMenuRequested(int,QPoint)));
}

QTabBar* TabWidget::tabBar() const
{
    return QTabWidget::tabBar();
}

QSize TabWidget::sizeHint() const
{
    if (tabBar()->testAttribute(Qt::WA_WState_Hidden))
    {
        QStackedWidget* stack = findChild<QStackedWidget*>("qt_tabwidget_stackedwidget");
        return stack->sizeHint();
    }
    return QTabWidget::sizeHint();
}

QColor TabWidget::tabTextColor(TabWidget::TabRole role) const
{
    return d.colors.value(role);
}

void TabWidget::setTabTextColor(TabWidget::TabRole role, const QColor& color)
{
    d.colors[role] = color;
    if (!d.updatingColors)
    {
        d.updatingColors = true;
        QMetaObject::invokeMethod(this, "updateTabColors", Qt::QueuedConnection);
    }
}

bool TabWidget::isTabInactive(int index)
{
    return d.inactiveIndexes.contains(index);
}

void TabWidget::setTabInactive(int index, bool inactive)
{
    if (!inactive)
        d.inactiveIndexes.removeAll(index);
    else if (!d.inactiveIndexes.contains(index))
        d.inactiveIndexes.append(index);
    colorizeTab(index);
}

bool TabWidget::hasTabAlert(int index)
{
    return d.alertIndexes.contains(index);
}

void TabWidget::setTabAlert(int index, bool alert)
{
    if (!alert)
    {
        int count = d.alertIndexes.removeAll(index);
        if (count > 0 && d.alertIndexes.isEmpty())
        {
            emit alertStatusChanged(false);
            SharedTimer::instance()->unregisterReceiver(this, "alertTimeout");
        }
    }
    else
    {
        if (d.alertIndexes.isEmpty())
        {
            emit alertStatusChanged(true);
            SharedTimer::instance()->registerReceiver(this, "alertTimeout");
        }
        if (!d.alertIndexes.contains(index))
            d.alertIndexes.append(index);
    }
    colorizeTab(index);
}

bool TabWidget::hasTabHighlight(int index) const
{
    return d.highlightIndexes.contains(index);
}

void TabWidget::setTabHighlight(int index, bool highlight)
{
    if (!highlight)
    {
        int count = d.highlightIndexes.removeAll(index);
        if (count > 0 && d.highlightIndexes.isEmpty())
            emit highlightStatusChanged(false);
    }
    else
    {
        if (d.highlightIndexes.isEmpty())
            emit highlightStatusChanged(true);
        if (!d.highlightIndexes.contains(index))
            d.highlightIndexes.append(index);
    }
    colorizeTab(index);
}

void TabWidget::moveToNextTab()
{
    int index = currentIndex();
    if (++index >= count() - 1)
        index = 0;
    setCurrentIndex(index);
}

void TabWidget::moveToPrevTab()
{
    int index = currentIndex();
    if (--index < 0)
        index = count() - 2;
    setCurrentIndex(index);
}

static void shiftIndexesFrom(QList<int>& indexes, int from, int delta)
{
    QMutableListIterator<int> it(indexes);
    while (it.hasNext())
    {
        if (it.next() >= from)
            it.value() += delta;
    }
}

void TabWidget::tabInserted(int index)
{
    shiftIndexesFrom(d.inactiveIndexes, index, 1);
    shiftIndexesFrom(d.alertIndexes, index, 1);
    shiftIndexesFrom(d.highlightIndexes, index, 1);
    colorizeTab(index);
}

void TabWidget::tabRemoved(int index)
{
    d.inactiveIndexes.removeAll(index);
    d.alertIndexes.removeAll(index);
    d.highlightIndexes.removeAll(index);
    shiftIndexesFrom(d.inactiveIndexes, index, -1);
    shiftIndexesFrom(d.alertIndexes, index, -1);
    shiftIndexesFrom(d.highlightIndexes, index, -1);
}

void TabWidget::tabChanged(int index)
{
    if (index == count() - 1)
    {
        emit newTabRequested();
        if (d.previous != -1)
            setCurrentIndex(d.previous);
    }
    else
    {
        d.previous = index;
    }
}

void TabWidget::alertTimeout()
{
    if (d.currentAlertColor == d.colors.value(Alert))
        d.currentAlertColor = d.colors.value(Active);
    else
        d.currentAlertColor = d.colors.value(Alert);

    foreach (int index, d.alertIndexes)
        colorizeTab(index);
}

void TabWidget::colorizeTab(int index)
{
    if (isTabInactive(index))
        tabBar()->setTabTextColor(index, d.colors.value(Inactive));
    else if (hasTabAlert(index))
        tabBar()->setTabTextColor(index, d.currentAlertColor);
    else if (hasTabHighlight(index))
        tabBar()->setTabTextColor(index, d.colors.value(Highlight));
    else
        tabBar()->setTabTextColor(index, d.colors.value(Active));
}

void TabWidget::updateTabColors()
{
    for (int i = 0; i < count(); ++i)
        colorizeTab(i);
    d.updatingColors = false;
}
