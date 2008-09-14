/*
* Copyright (C) 2008 J-P Nurmi jpnurmi@gmail.com
* Copyright (C) 2008 Adam Higerd ahigerd@libqxt.org
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* $Id$
*/

#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include "ui_connectdialog.h"

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectDialog(QWidget* parent = 0);

    QString host() const;
    quint16 port() const;
    QString nick() const;
    QString pass() const;

public slots:
    virtual void accept();

private:
    Ui::ConnectDialog ui;
};

#endif // CONNECTDIALOG_H
