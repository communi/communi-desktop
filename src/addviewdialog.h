/*
* Copyright (C) 2008-2013 The Communi Project
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

#ifndef ADDVIEWDIALOG_H
#define ADDVIEWDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class IrcConnection;
class QDialogButtonBox;

class AddViewDialog : public QDialog
{
    Q_OBJECT

public:
    AddViewDialog(IrcConnection* connection, QWidget* parent = 0);

    bool isChannel() const;
    QString view() const;
    QString password() const;

private slots:
    void updateUi();

private:
    struct Private {
        IrcConnection* connection;
        QLabel* viewLabel;
        QLabel* passLabel;
        QLineEdit* viewEdit;
        QLineEdit* passEdit;
        QDialogButtonBox* buttonBox;
    } d;
};

#endif // ADDVIEWDIALOG_H
