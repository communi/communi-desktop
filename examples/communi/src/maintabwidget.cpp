/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#include "maintabwidget.h"
#include "application.h"
#include "settings.h"
#include <QTabBar>

MainTabWidget::MainTabWidget(QWidget* parent) : TabWidget(parent)
{
    setTabPosition(QTabWidget::West);
    setStyleSheet(".MainTabWidget::pane { border: 0px; }");

#if QT_VERSION >= 0x040600
    registerSwipeGestures(Qt::Vertical);
#endif

    connect(qApp, SIGNAL(settingsChanged(Settings)), this, SLOT(applySettings(Settings)));
    applySettings(Application::settings());
}

void MainTabWidget::applySettings(const Settings& settings)
{
    setAlertColor(QColor(settings.colors.value(Settings::Highlight)));
    setHighlightColor(QColor(settings.colors.value(Settings::Highlight)));
}

void MainTabWidget::setSessionTitle(const QString& title)
{
    int index = senderIndex();
    if (index != -1)
        setTabText(index, title);
}

void MainTabWidget::activateAlert(bool active)
{
    int index = senderIndex();
    if (index != -1)
        setTabAlert(index, active);
}

void MainTabWidget::activateHighlight(bool active)
{
    int index = senderIndex();
    if (index != -1)
        setTabHighlight(index, active);
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
