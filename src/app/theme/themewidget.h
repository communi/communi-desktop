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
#include <QGroupBox>
#include <QGridLayout>
#include "themeinfo.h"

class ChatPage;
class ThemeInfo;

class ThemeWidget : public QGroupBox
{
    Q_OBJECT

public:
    ThemeWidget(const ThemeInfo& theme, QWidget* parent = 0);
    ~ThemeWidget();

    ThemeInfo theme() const;
    bool isSelected() const;

public slots:
    void setSelected(bool selected);

signals:
    void selected(const ThemeInfo& theme);

protected:
    bool eventFilter(QObject* object, QEvent* event);

private slots:
    void updatePreview();

private:
    struct Private {
        bool selected;
        ThemeInfo theme;
        ChatPage* page;
        QLabel* preview;
        QGridLayout* grid;
    } d;
};

#endif // THEMEWIDGET_H
