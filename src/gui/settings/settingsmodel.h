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

#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QAbstractTableModel>

class SettingsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit SettingsModel(QObject* parent = 0);
    virtual ~SettingsModel();

    QVariant value(const QString& key) const;
    void setValue(const QString& key, const QVariant& value);

    QVariantMap values(const QString& pattern = QString()) const;
    void setValues(const QVariantMap& values);

    Qt::ItemFlags flags(const QModelIndex& index) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

signals:
    void changed();

private:
    QVariantMap d;
};

#endif // SETTINGSMODEL_H
