/*
* Copyright (C) 2008-2014 The Communi Project
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

class IrcBuffer;
class TextShadow;
class TextDocument;

class TextBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    explicit TextBrowser(QWidget* parent = 0);
    ~TextBrowser();

    IrcBuffer* buffer() const;

    TextDocument* document() const;
    void setDocument(TextDocument* document);

    QWidget* buddy() const;
    void setBuddy(QWidget* buddy);

    bool isAtTop() const;
    bool isAtBottom() const;
    bool isZoomed() const;

    QMenu* createContextMenu(const QPoint& pos);

public slots:
    void clear();
    void resetZoom();
    void scrollToTop();
    void scrollToBottom();
    void scrollToNextPage();
    void scrollToPreviousPage();
    void moveCursorToBottom();

signals:
    void queried(const QString& user);
    void documentChanged(TextDocument* document);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* event);

private slots:
    void keepAtBottom();
    void keepPosition(int delta);
    void moveShadow(int offset);
    void onAnchorClicked(const QUrl& url);

    void onWhoisTriggered();
    void onQueryTriggered();

private:
    struct Private {
        bool events;
        QWidget* bud;
        TextShadow* shadow;
    } d;
};

#endif // TEXTBROWSER_H
