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

#ifndef TEXTBROWSER_H
#define TEXTBROWSER_H

#include <QTextBrowser>

class TextBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    TextBrowser(QWidget* parent = 0);

    QWidget* buddy() const;
    void setBuddy(QWidget* buddy);

    void addMarker(int block);
    void removeMarker(int block);

    QColor highlightColor() const;
    void setHighlightColor(const QColor& color);

    QColor markerColor() const;
    void setMarkerColor(const QColor& color);

    QColor shadowColor() const;
    void setShadowColor(const QColor& color);

public slots:
    void append(const QString& text, bool highlight = false);

    void scrollToTop();
    void scrollToBottom();
    void scrollToNextPage();
    void scrollToPreviousPage();

protected:
    void hideEvent(QHideEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* event);

    void paintMarker(QPainter* painter, const QTextBlock& block, const QColor& color);

private:
    struct Private {
        int ub;
        QWidget* bud;
        QList<int> markers;
        QList<int> highlights;
        QColor highlightColor;
        QColor markerColor;
        QColor shadowColor;
    } d;
};

#endif // TEXTBROWSER_H
