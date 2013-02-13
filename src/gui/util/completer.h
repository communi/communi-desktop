/*
* Copyright (C) 2008-2013 Communi authors
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

#ifndef COMPLETER_H
#define COMPLETER_H

#include <QCompleter>
class HistoryLineEdit;

class Completer : public QCompleter
{
    Q_OBJECT

public:
    Completer(QObject* parent = 0);

    HistoryLineEdit* lineEdit() const;
    void setLineEdit(HistoryLineEdit* lineEdit);

    QAbstractItemModel* userModel() const;
    void setUserModel(QAbstractItemModel* model);

    QAbstractItemModel* commandModel() const;
    void setCommandModel(QAbstractItemModel* model);

private slots:
    void onTabPressed();
    void onTextEdited();
    void insertCompletion(const QString& completion);

private:
    struct CompleterData {
        HistoryLineEdit* lineEdit;
        QAbstractItemModel* userModel;
        QAbstractItemModel* commandModel;
    } d;
};

#endif // COMPLETER_H
