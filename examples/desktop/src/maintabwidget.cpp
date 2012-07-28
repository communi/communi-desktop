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

#include "maintabwidget.h"
#include "sessiontabwidget.h"
#include "settings.h"
#include <QShortcut>
#include <QTabBar>

MainTabWidget::MainTabWidget(QWidget* parent) : TabWidget(parent)
{
    setTabPosition(QTabWidget::West);
    setStyleSheet(".MainTabWidget::pane { border: 0px; }");

    d.tabUpShortcut = new QShortcut(this);
    connect(d.tabUpShortcut, SIGNAL(activated()), this, SLOT(moveToPrevTab()));

    d.tabDownShortcut = new QShortcut(this);
    connect(d.tabDownShortcut, SIGNAL(activated()), this, SLOT(moveToNextTab()));

    d.tabLeftShortcut = new QShortcut(this);
    connect(d.tabLeftShortcut, SIGNAL(activated()), this, SLOT(moveToPrevSubTab()));

    d.tabRightShortcut = new QShortcut(this);
    connect(d.tabRightShortcut, SIGNAL(activated()), this, SLOT(moveToNextSubTab()));

    QShortcut* shortcut = new QShortcut(QKeySequence::New, this);
    connect(shortcut, SIGNAL(activated()), this, SIGNAL(newTabRequested()));
}

void MainTabWidget::tabInserted(int index)
{
    TabWidget::tabInserted(index);
    tabBar()->setVisible(count() > 2);
}

void MainTabWidget::tabRemoved(int index)
{
    TabWidget::tabRemoved(index);
    tabBar()->setVisible(count() > 2);
}

void MainTabWidget::applySettings(const Settings& settings)
{
    d.tabUpShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::TabUp)));
    d.tabDownShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::TabDown)));
    d.tabLeftShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::TabLeft)));
    d.tabRightShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::TabRight)));

    QColor color(settings.colors.value(Settings::Highlight));
    setAlertColor(color);
    setHighlightColor(color);

    for (int i = 0; i < count(); ++i)
    {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(i));
        if (tabWidget)
            tabWidget->applySettings(settings);
    }
}

void MainTabWidget::setSessionTitle(const QString& title)
{
    int index = senderIndex();
    if (index != -1)
        setTabText(index, title);
}

void MainTabWidget::setInactive(bool inactive)
{
    int index = senderIndex();
    if (index != -1)
        setTabInactive(index, inactive);
}

void MainTabWidget::setAlerted(bool alerted)
{
    int index = senderIndex();
    if (index != -1)
        setTabAlert(index, alerted);
}

void MainTabWidget::setHighlighted(bool highlighted)
{
    int index = senderIndex();
    if (index != -1)
        setTabHighlight(index, highlighted);
}

int MainTabWidget::senderIndex() const
{
    if (!sender() || !sender()->isWidgetType())
        return -1;

    return indexOf(static_cast<QWidget*>(sender()));
}

void MainTabWidget::moveToNextSubTab()
{
    TabWidget* tabWidget = qobject_cast<TabWidget*>(currentWidget());
    if (tabWidget)
        tabWidget->moveToNextTab();
}

void MainTabWidget::moveToPrevSubTab()
{
    TabWidget* tabWidget = qobject_cast<TabWidget*>(currentWidget());
    if (tabWidget)
        tabWidget->moveToPrevTab();
}
