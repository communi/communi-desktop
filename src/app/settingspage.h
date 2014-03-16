/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "ui_settingspage.h"
#include "themeinfo.h"

class ThemeWidget;

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    SettingsPage(QWidget* parent = 0);
    ~SettingsPage();

    QString theme() const;

public slots:
    void setTheme(const QString& theme);

signals:
    void accepted();
    void rejected();

private slots:
    void select(const ThemeInfo& theme);

private:
    struct Private : public Ui::SettingsPage {
        QString theme;
        QList<ThemeWidget*> widgets;
    } ui;
};

#endif // SETTINGSPAGE_H
