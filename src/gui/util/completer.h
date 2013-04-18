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

    QAbstractItemModel* channelModel() const;
    void setChannelModel(QAbstractItemModel* model);

    QAbstractItemModel* commandModel() const;
    void setCommandModel(QAbstractItemModel* model);

    QString channelPrefixes() const;
    void setChannelPrefixes(const QString& prefixes);

    QString commandPrefixes() const;
    void setCommandPrefixes(const QString& prefixes);

signals:
    void commandCompletion(const QString& command);

private slots:
    void onTabPressed();
    void onTextEdited();
    void insertCompletion(const QString& completion);

private:
    struct Private {
        HistoryLineEdit* lineEdit;
        QAbstractItemModel* userModel;
        QAbstractItemModel* channelModel;
        QAbstractItemModel* commandModel;
        QString channelPrefixes;
        QString commandPrefixes;
    } d;
};

#endif // COMPLETER_H
