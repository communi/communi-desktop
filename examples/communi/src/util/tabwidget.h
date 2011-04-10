/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QColor>
#include <QList>
#if QT_VERSION >= 0x040600
#include <QSwipeGesture>
#endif // QT_VERSION

class TabWidget : public QTabWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor alertColor READ alertColor WRITE setAlertColor)
    Q_PROPERTY(QColor highlightColor READ highlightColor WRITE setHighlightColor)

public:
    TabWidget(QWidget* parent = 0);

    QColor alertColor() const;
    void setAlertColor(const QColor& color);

    QColor highlightColor() const;
    void setHighlightColor(const QColor& color);

    bool hasTabAlert(int index);
    void setTabAlert(int index, bool alert);

    bool hasTabHighlight(int index) const;
    void setTabHighlight(int index, bool highlight);

#if QT_VERSION >= 0x040600
    void registerSwipeGestures(Qt::Orientation orientation);
    void unregisterSwipeGestures();
#endif // QT_VERSION

public slots:
    void moveToNextTab();
    void moveToPrevTab();
    void setTabBarVisible(bool visible);

signals:
    void alertStatusChanged(bool active);
    void highlightStatusChanged(bool active);

protected:
#if QT_VERSION >= 0x040600
    bool event(QEvent* event);
    bool handleSwipeGesture(QSwipeGesture* gesture, QSwipeGesture::SwipeDirection direction);
#endif // QT_VERSION
    void tabInserted(int index);
    void tabRemoved(int index);

private slots:
    void alertTimeout();

private:
    struct TabWidgetData
    {
        QColor alertColor;
        QColor highlightColor;
        QList<int> alertIndexes;
        QList<int> highlightIndexes;
        Qt::Orientation swipeOrientation;
    } d;
};

#endif // TABWIDGET_H
