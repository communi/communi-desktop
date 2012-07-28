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
#include "session.h"
#include <QShortcut>
#include <QTabBar>

MainTabWidget::MainTabWidget(QWidget* parent) : TabWidget(parent)
{
    setTabPosition(QTabWidget::West);
    setStyleSheet(".MainTabWidget::pane { border: 0px; }");

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
    connect(this, SIGNAL(tabMenuRequested(int,QPoint)), this, SLOT(onTabMenuRequested(int,QPoint)));

    d.tabUpShortcut = new QShortcut(this);
    connect(d.tabUpShortcut, SIGNAL(activated()), this, SLOT(moveToPrevTab()));

    d.tabDownShortcut = new QShortcut(this);
    connect(d.tabDownShortcut, SIGNAL(activated()), this, SLOT(moveToNextTab()));

    QShortcut* shortcut = new QShortcut(QKeySequence::New, this);
    connect(shortcut, SIGNAL(activated()), this, SIGNAL(newTabRequested()));

    applySettings(d.settings);
}

QList<Session*> MainTabWidget::sessions() const
{
    QList<Session*> list;
    for (int i = 0; i < count(); ++i)
    {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(i));
        if (tabWidget)
            list += tabWidget->session();
    }
    return list;
}

void MainTabWidget::addSession(Session* session, const QString& name)
{
    SessionTabWidget* tab = new SessionTabWidget(session, this);
    if (name.isEmpty())
        connect(tab, SIGNAL(titleChanged(QString)), this, SLOT(setSessionTitle(QString)));
    connect(tab, SIGNAL(inactiveStatusChanged(bool)), this, SLOT(setInactive(bool)));
    connect(tab, SIGNAL(alertStatusChanged(bool)), this, SLOT(setAlerted(bool)));
    connect(tab, SIGNAL(highlightStatusChanged(bool)), this, SLOT(setHighlighted(bool)));

    int index = addTab(tab, name.isEmpty() ? session->host() : name);
    setCurrentIndex(index);
    setTabInactive(index, !session->isActive());
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

void MainTabWidget::tabActivated(int index)
{
    if (index < count() - 1)
    {
        SessionTabWidget* tab = qobject_cast<SessionTabWidget*>(widget(index));
        if (tab)
        {
            setWindowFilePath(tab->tabText(tab->currentIndex()));
            QMetaObject::invokeMethod(tab, "delayedTabReset");
        }
    }
}

void MainTabWidget::applySettings(const Settings& settings)
{
    d.settings = settings;
    d.tabUpShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::TabUp)));
    d.tabDownShortcut->setKey(QKeySequence(settings.shortcuts.value(Settings::TabDown)));

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

void MainTabWidget::onTabMenuRequested(int index, const QPoint& pos)
{
    if (index < count() - 1)
    {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(index));
        if (tabWidget)
            QMetaObject::invokeMethod(tabWidget, "onTabMenuRequested", Q_ARG(int, 0), Q_ARG(QPoint, pos));
    }
}
