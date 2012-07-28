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

#ifndef MAINTABWIDGET_H
#define MAINTABWIDGET_H

#include "tabwidget.h"

struct Settings;
class QShortcut;

class MainTabWidget : public TabWidget
{
    Q_OBJECT

public:
    MainTabWidget(QWidget* parent = 0);

public slots:
    void applySettings(const Settings& settings);

protected:
    void tabInserted(int index);
    void tabRemoved(int index);

private slots:
    void setSessionTitle(const QString& title);
    void setInactive(bool inactive);
    void setAlerted(bool alerted);
    void setHighlighted(bool highlighted);
    void moveToNextSubTab();
    void moveToPrevSubTab();

private:
    int senderIndex() const;

    struct MainTabWidgetData
    {
        QShortcut* tabUpShortcut;
        QShortcut* tabDownShortcut;
        QShortcut* tabLeftShortcut;
        QShortcut* tabRightShortcut;
    } d;
};

#endif // MAINTABWIDGET_H
