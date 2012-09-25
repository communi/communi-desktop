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

#include "multisessiontabwidget.h"
#include "sessiontabwidget.h"
#include "tabwidget_p.h"
#include "settings.h"
#include "session.h"
#include <QShortcut>
#include <QTabBar>

MultiSessionTabWidget::MultiSessionTabWidget(QWidget* parent) : TabWidget(parent)
{
    setTabPosition(QTabWidget::West);
    setStyleSheet(".MainTabWidget::pane { border: 0px; }");

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabActivated(int)));
    connect(this, SIGNAL(tabMenuRequested(int,QPoint)), this, SLOT(onTabMenuRequested(int,QPoint)));

    QShortcut* shortcut = new QShortcut(QKeySequence::New, this);
    connect(shortcut, SIGNAL(activated()), this, SIGNAL(newTabRequested()));

    applySettings(d.settings);
}

QList<Session*> MultiSessionTabWidget::sessions() const
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

void MultiSessionTabWidget::addSession(Session* session)
{
    SessionTabWidget* tab = new SessionTabWidget(session, this);
    connect(tab, SIGNAL(highlightStatusChanged(bool)), this, SLOT(setHighlighted(bool)));
    connect(tab, SIGNAL(inactiveStatusChanged(bool)), this, SLOT(setInactive(bool)));
    connect(tab, SIGNAL(alertStatusChanged(bool)), this, SLOT(setAlerted(bool)));
    connect(tab, SIGNAL(alerted(MessageView*,IrcMessage*)), this, SIGNAL(alerted(MessageView*,IrcMessage*)));
    connect(tab, SIGNAL(highlighted(MessageView*,IrcMessage*)), this, SIGNAL(highlighted(MessageView*,IrcMessage*)));
    connect(tab, SIGNAL(sessionClosed(Session*)), this, SIGNAL(sessionRemoved(Session*)));
    connect(tab, SIGNAL(splitterChanged(QByteArray)), this, SLOT(restoreSplitter(QByteArray)));
    tab->applySettings(d.settings);

    QString name = session->name();
    if (name.isEmpty())
        connect(session, SIGNAL(networkChanged(QString)), this, SLOT(onSessionNetworkChanged(QString)));
    int index = addTab(tab, name.isEmpty() ? session->host() : name);
    setCurrentIndex(index);
    setTabInactive(index, !session->isActive());

    emit sessionAdded(session);
}

void MultiSessionTabWidget::removeSession(Session *session)
{
    SessionTabWidget* tabWidget = sessionWidget(session);
    if (tabWidget)
        tabWidget->deleteLater();
    emit sessionRemoved(session);
}

SessionTabWidget* MultiSessionTabWidget::sessionWidget(Session* session) const
{
    for (int i = 0; i < count(); ++i)
    {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(i));
        if (tabWidget && tabWidget->session() == session)
            return tabWidget;
    }
    return 0;
}

void MultiSessionTabWidget::tabInserted(int index)
{
    TabWidget::tabInserted(index);
    tabBar()->setVisible(d.settings.layout == "tabs" && count() > 2);
}

void MultiSessionTabWidget::tabRemoved(int index)
{
    TabWidget::tabRemoved(index);
    tabBar()->setVisible(d.settings.layout == "tabs" && count() > 2);
}

void MultiSessionTabWidget::tabActivated(int index)
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

void MultiSessionTabWidget::applySettings(const Settings& settings)
{
    d.settings = settings;

    TabBar* tb = static_cast<TabBar*>(tabBar());
    tb->setNavigationShortcut(TabBar::Previous, QKeySequence(settings.shortcuts.value(Settings::TabUp)));
    tb->setNavigationShortcut(TabBar::Next, QKeySequence(settings.shortcuts.value(Settings::TabDown)));
    tb->setVisible(d.settings.layout == "tabs" && count() > 2);

    QColor color(settings.colors.value(Settings::Highlight));
    setTabTextColor(Alert, color);
    setTabTextColor(Highlight, color);

    for (int i = 0; i < count(); ++i)
    {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(i));
        if (tabWidget)
            tabWidget->applySettings(settings);
    }
}

QByteArray MultiSessionTabWidget::saveSplitter() const
{
    QByteArray state;
    for (int i = count() - 1; state.isNull() && i >= 0; --i)
    {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(i));
        if (tabWidget)
            state = tabWidget->saveSplitter();
    }
    return state;
}

void MultiSessionTabWidget::restoreSplitter(const QByteArray& state)
{
    for (int i = 0; i < count(); ++i)
    {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(i));
        if (tabWidget)
        {
            tabWidget->blockSignals(true);
            tabWidget->restoreSplitter(state);
            tabWidget->blockSignals(false);
        }
    }
    emit splitterChanged(state);
}

void MultiSessionTabWidget::onSessionNetworkChanged(const QString& network)
{
    Session* session = qobject_cast<Session*>(sender());
    SessionTabWidget* tabWidget = sessionWidget(session);
    if (tabWidget)
        setTabText(indexOf(tabWidget), network);
}

void MultiSessionTabWidget::setInactive(bool inactive)
{
    int index = senderIndex();
    if (index != -1)
        setTabInactive(index, inactive);
}

void MultiSessionTabWidget::setAlerted(bool alerted)
{
    int index = senderIndex();
    if (index != -1)
        setTabAlert(index, alerted);
}

void MultiSessionTabWidget::setHighlighted(bool highlighted)
{
    int index = senderIndex();
    if (index != -1)
        setTabHighlight(index, highlighted);
}

int MultiSessionTabWidget::senderIndex() const
{
    if (!sender() || !sender()->isWidgetType())
        return -1;

    return indexOf(static_cast<QWidget*>(sender()));
}

void MultiSessionTabWidget::onTabMenuRequested(int index, const QPoint& pos)
{
    if (index < count() - 1)
    {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(index));
        if (tabWidget)
            QMetaObject::invokeMethod(tabWidget, "onTabMenuRequested", Q_ARG(int, 0), Q_ARG(QPoint, pos));
    }
}
