/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include <QLabel>

class ChatPage;

class ThemeWidget : public QLabel
{
    Q_OBJECT

public:
    ThemeWidget(QWidget* parent = 0);
    ~ThemeWidget();

    QString theme() const;

public slots:
    void setTheme(const QString& theme);

protected:
    void resizeEvent(QResizeEvent* event);

private slots:
    void updatePreview();

private:
    struct Private {
        ChatPage* page;
    } d;
};

#endif // THEMEWIDGET_H
