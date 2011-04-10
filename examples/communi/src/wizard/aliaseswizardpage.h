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

#ifndef ALIASESWIZARDPAGE_H
#define ALIASESWIZARDPAGE_H

#include "ui_aliaseswizardpage.h"
class QSortFilterProxyModel;
class QStandardItemModel;

class AliasesWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    AliasesWizardPage(QWidget* parent = 0);

    QMap<QString, QString> aliases() const;
    void setAliases(const QMap<QString, QString>& aliases);

private slots:
    void addAlias();
    void editAlias();
    void removeAlias();
    void updateUi();

private:
    Ui::AliasesWizardPage ui;
    QStandardItemModel* sourceModel;
    QSortFilterProxyModel* proxyModel;
};

#endif // ALIASESWIZARDPAGE_H
