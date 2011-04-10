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

#ifndef USERWIZARDPAGE_H
#define USERWIZARDPAGE_H

#include "ui_userwizardpage.h"

class UserWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    UserWizardPage(QWidget* parent = 0);
    ~UserWizardPage();

    QString nickName() const;
    void setNickName(const QString& nickName);

    QString realName() const;
    void setRealName(const QString& realName);

    bool isComplete() const;

private:
    Ui::UserWizardPage ui;
};

#endif // USERWIZARDPAGE_H
