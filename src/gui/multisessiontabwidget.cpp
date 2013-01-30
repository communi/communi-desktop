/*
* Copyright (C) 2008-2013 J-P Nurmi <jpnurmi@gmail.com>
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
#include "messageview.h"
#include "settings.h"
#include "session.h"
#include <QShortcut>
#include <QTabBar>

MultiSessionTabWidget::MultiSessionTabWidget(QWidget* parent) : QStackedWidget(parent)
{
    setStyleSheet(".MultiSessionTabWidget::pane { border: 0px; }");

    QShortcut* shortcut = new QShortcut(QKeySequence::New, this);
    connect(shortcut, SIGNAL(activated()), this, SIGNAL(newTabRequested()));

    applySettings(d.settings);
}

QList<Session*> MultiSessionTabWidget::sessions() const
{
    QList<Session*> list;
    for (int i = 0; i < count(); ++i) {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(i));
        if (tabWidget)
            list += tabWidget->session();
    }
    return list;
}

void MultiSessionTabWidget::addSession(Session* session)
{
    SessionTabWidget* tab = new SessionTabWidget(session, this);
    connect(tab, SIGNAL(alerted(MessageView*, IrcMessage*)), this, SIGNAL(alerted(MessageView*, IrcMessage*)));
    connect(tab, SIGNAL(highlighted(MessageView*, IrcMessage*)), this, SIGNAL(highlighted(MessageView*, IrcMessage*)));
    connect(tab, SIGNAL(sessionClosed(Session*)), this, SLOT(removeSession(Session*)));
    connect(tab, SIGNAL(splitterChanged(QByteArray)), this, SLOT(restoreSplitter(QByteArray)));
    tab->applySettings(d.settings);

    int index = addWidget(tab);
    setCurrentIndex(index);

    emit sessionAdded(session);
}

void MultiSessionTabWidget::removeSession(Session* session)
{
    SessionTabWidget* tabWidget = sessionWidget(session);
    if (tabWidget) {
        removeWidget(tabWidget);
        tabWidget->deleteLater();
        emit sessionRemoved(session);
    }
}

SessionTabWidget* MultiSessionTabWidget::sessionWidget(Session* session) const
{
    for (int i = 0; i < count(); ++i) {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(i));
        if (tabWidget && tabWidget->session() == session)
            return tabWidget;
    }
    return 0;
}

void MultiSessionTabWidget::applySettings(const Settings& settings)
{
    d.settings = settings;

    for (int i = 0; i < count(); ++i) {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(i));
        if (tabWidget)
            tabWidget->applySettings(settings);
    }
}

QByteArray MultiSessionTabWidget::saveSplitter() const
{
    QByteArray state;
    for (int i = count(); state.isNull() && i >= 0; --i) {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(i));
        if (tabWidget)
            state = tabWidget->saveSplitter();
    }
    return state;
}

void MultiSessionTabWidget::restoreSplitter(const QByteArray& state)
{
    for (int i = 0; i < count(); ++i) {
        SessionTabWidget* tabWidget = qobject_cast<SessionTabWidget*>(widget(i));
        if (tabWidget) {
            tabWidget->blockSignals(true);
            tabWidget->restoreSplitter(state);
            tabWidget->blockSignals(false);
        }
    }
    emit splitterChanged(state);
}
