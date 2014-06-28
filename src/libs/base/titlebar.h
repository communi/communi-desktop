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

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QLabel>
#include <QTextEdit>
#include <QToolButton>

class IrcBuffer;
class MessageFormatter;

class TitleBar : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)

public:
    explicit TitleBar(QWidget* parent = 0);

    IrcBuffer* buffer() const;
    QString topic() const;

    QMenu* menu() const;

    QSize minimumSizeHint() const;
    int heightForWidth(int width) const;

    int offset() const;
    int baseOffset() const;

    QString styleSheet() const;
    void setStyleSheet(const QString& css);

public slots:
    void setBuffer(IrcBuffer* buffer);
    void setTopic(const QString& topic);

    void expand();
    void collapse();
    void setOffset(int offset);

signals:
    void offsetChanged(int offset);
    void bufferChanged(IrcBuffer* buffer);

protected:
    bool event(QEvent* event);
    bool eventFilter(QObject* object, QEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);

private slots:
    void relayout();
    void cleanup();
    void refresh();
    void edit();

private:
    int topMargin() const;

    struct Private {
        QString css;
        int baseOffset;
        IrcBuffer* buffer;
        QTextEdit* editor;
        QToolButton* menuButton;
        MessageFormatter* formatter;
    } d;
};

#endif // TITLEBAR_H
