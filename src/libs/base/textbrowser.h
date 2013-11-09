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

class TextDocument;

class TextBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    explicit TextBrowser(QWidget* parent = 0);
    ~TextBrowser();

    TextDocument* document() const;
    void setDocument(TextDocument* document);

    QWidget* buddy() const;
    void setBuddy(QWidget* buddy);

    QColor markerColor() const;
    void setMarkerColor(const QColor& color);

    QColor highlightColor() const;
    void setHighlightColor(const QColor& color);

    bool isAtTop() const;
    bool isAtBottom() const;

public slots:
    void scrollToTop();
    void scrollToBottom();
    void scrollToNextPage();
    void scrollToPreviousPage();

signals:
    void queried(const QString& user);
    void documentChanged(TextDocument* document);

protected:
    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void timerEvent(QTimerEvent* event);
    void wheelEvent(QWheelEvent* event);

private slots:
    void onAnchorClicked(const QUrl& url);

private:
    struct Private {
        int dirty;
        QWidget* bud;
        QColor markerColor;
        QColor highlightColor;
    } d;
};

#endif // TEXTBROWSER_H
