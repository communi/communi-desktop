/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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
#include "sharedtimer.h"
#include <QTabBar>
#if QT_VERSION >= 0x040600
#include <QGestureEvent>
#endif // QT_VERSION

class TabBar : public QTabBar
{
public:
    TabBar(QWidget* parent = 0) : QTabBar(parent)
    {
    }

protected:
    void changeEvent(QEvent* event)
    {
        if (event->type() == QEvent::StyleChange)
        {
            Qt::TextElideMode mode = elideMode();
            QTabBar::changeEvent(event);
            if (mode != elideMode()) {
                setElideMode(mode);
            }
            return;
        }
        QTabBar::changeEvent(event);
    }
};

TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent)
{
    setTabBar(new TabBar(this));
    setElideMode(Qt::ElideMiddle);
    d.alertColor = palette().color(QPalette::Highlight);
    d.highlightColor = palette().color(QPalette::Highlight);
    d.swipeOrientation = Qt::Orientation(0);
}

QColor TabWidget::alertColor() const
{
    return d.alertColor;
}

void TabWidget::setAlertColor(const QColor& color)
{
    d.alertColor = color;
}

QColor TabWidget::highlightColor() const
{
    return d.highlightColor;
}

void TabWidget::setHighlightColor(const QColor& color)
{
    d.highlightColor = color;
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
            setTabHighlight(index, hasTabHighlight(index));
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
        tabBar()->setTabTextColor(index, QColor());
    }
    else
    {
        if (d.highlightIndexes.isEmpty())
            emit highlightStatusChanged(true);
        if (!d.highlightIndexes.contains(index))
            d.highlightIndexes.append(index);
        tabBar()->setTabTextColor(index, d.highlightColor);
    }
}

void TabWidget::registerSwipeGestures(Qt::Orientation orientation)
{
    if (d.swipeOrientation != 0) {
        qWarning("TabWidget::registerSwipeGestures: already registered");
        return;
    }

    d.swipeOrientation = orientation;
    grabGesture(Qt::SwipeGesture);
}

void TabWidget::unregisterSwipeGestures()
{
    if (d.swipeOrientation == 0) {
        qWarning("TabWidget::unregisterSwipeGestures: not registered");
        return;
    }

    d.swipeOrientation = Qt::Orientation(0);
    ungrabGesture(Qt::SwipeGesture);
}

void TabWidget::moveToNextTab()
{
    int index = currentIndex();
    if (++index >= count())
        index = 0;
    setCurrentIndex(index);
}

void TabWidget::moveToPrevTab()
{
    int index = currentIndex();
    if (--index < 0)
        index = count() - 1;
    setCurrentIndex(index);
}

void TabWidget::setTabBarVisible(bool visible)
{
    tabBar()->setVisible(visible);
}

#if QT_VERSION >= 0x040600
bool TabWidget::event(QEvent* event)
{
    if (event->type() == QEvent::Gesture)
    {
        QGestureEvent* gestureEvent = static_cast<QGestureEvent*>(event);
        QGesture* gesture = gestureEvent->gesture(Qt::SwipeGesture);
        QSwipeGesture* swipeGesture = static_cast<QSwipeGesture*>(gesture);
        if (swipeGesture)
        {
            bool accepted = false;
            switch (d.swipeOrientation)
            {
                case Qt::Horizontal:
                    accepted = handleSwipeGesture(swipeGesture, swipeGesture->horizontalDirection());
                    break;
                case Qt::Vertical:
                    accepted = handleSwipeGesture(swipeGesture, swipeGesture->verticalDirection());
                    break;
                default:
                    Q_ASSERT(false);
                    break;
            }
            gestureEvent->setAccepted(swipeGesture, accepted);
        }
    }
    return QTabWidget::event(event);
}

bool TabWidget::handleSwipeGesture(QSwipeGesture* gesture, QSwipeGesture::SwipeDirection direction)
{
    switch (direction)
    {
        case QSwipeGesture::Up:
        case QSwipeGesture::Left:
            if (gesture->state() == Qt::GestureFinished)
                moveToPrevTab();
            return true;
        case QSwipeGesture::Down:
        case QSwipeGesture::Right:
            if (gesture->state() == Qt::GestureFinished)
                moveToNextTab();
            return true;
        default:
            return false;
    }
}
#endif // QT_VERSION

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
    shiftIndexesFrom(d.alertIndexes, index, 1);
    shiftIndexesFrom(d.highlightIndexes, index, 1);
}

void TabWidget::tabRemoved(int index)
{
    shiftIndexesFrom(d.alertIndexes, index, -1);
    shiftIndexesFrom(d.highlightIndexes, index, -1);
}

void TabWidget::alertTimeout()
{
    QColor color;
    if (!d.alertIndexes.isEmpty())
    {
        QColor tabColor = tabBar()->tabTextColor(d.alertIndexes.first());
        if (tabColor != d.alertColor)
            color = d.alertColor;
    }

    foreach (int index, d.alertIndexes)
        tabBar()->setTabTextColor(index, color);
}
