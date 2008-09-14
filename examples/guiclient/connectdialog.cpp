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

#include "connectdialog.h"
#include <QSettings>

ConnectDialog::ConnectDialog(QWidget* parent) : QDialog(parent)
{
    ui.setupUi(this);

    QSettings settings;
    if (settings.contains("host"))
        ui.lineEditHost->setText(settings.value("host").toString());
    if (settings.contains("port"))
        ui.spinBoxPort->setValue(settings.value("port").toInt());
    if (settings.contains("nick"))
        ui.lineEditNick->setText(settings.value("nick").toString());
    if (settings.contains("pass"))
        ui.lineEditPass->setText(settings.value("pass").toString());
}

void ConnectDialog::accept()
{
    QSettings settings;
    settings.setValue("host", host());
    settings.setValue("port", port());
    settings.setValue("nick", nick());
    settings.setValue("pass", pass());
    QDialog::accept();
}

QString ConnectDialog::host() const
{
    return ui.lineEditHost->text();
}

quint16 ConnectDialog::port() const
{
    return ui.spinBoxPort->text().toInt();
}

QString ConnectDialog::nick() const
{
    return ui.lineEditNick->text();
}

QString ConnectDialog::pass() const
{
    return ui.lineEditPass->text();
}
