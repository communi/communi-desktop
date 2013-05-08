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

#include "settingsmodel.h"
#include <QRegExp>

SettingsModel::SettingsModel(QObject* parent) : QAbstractTableModel(parent)
{
    connect(this, SIGNAL(modelReset()), this, SIGNAL(changed()));
}

SettingsModel::~SettingsModel()
{
    d.clear();
}

QVariant SettingsModel::value(const QString& key) const
{
    return d.value(key);
}

void SettingsModel::setValue(const QString& key, const QVariant& value)
{
    if (d.value(key) != value) {
        QModelIndex idx = index(d.keys().indexOf(key), 1);
        d.insert(key, value);
        emit dataChanged(idx, idx);
    }
}

QVariantMap SettingsModel::values(const QString& pattern) const
{
    if (!pattern.isEmpty()) {
        QVariantMap result;
        QMapIterator<QString,QVariant> it(d);
        QRegExp rx(pattern, Qt::CaseSensitive, QRegExp::Wildcard);
        while (it.hasNext()) {
            it.next();
            if (rx.exactMatch(it.key()))
                result[it.key()] = it.value();
        }
        return result;
    }
    return d;
}

void SettingsModel::setValues(const QVariantMap& values)
{
    if (d != values) {
        beginResetModel();
        QMapIterator<QString,QVariant> it(values);
        while (it.hasNext()) {
            it.next();
            d[it.key()] = it.value();
        }
        endResetModel();
    }
}

Qt::ItemFlags SettingsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == 1)
        f |= Qt::ItemIsEditable;
    return f;
}

int SettingsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return d.count();
}

int SettingsModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant SettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    return section == 0 ? tr("Key") : tr("Value");
}

QVariant SettingsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= d.count())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QString key = d.keys().at(index.row());
        return index.column() == 0 ? key : d.value(key);
    } else if (index.column() == 1 && role == Qt::DecorationRole) {
        QString key = d.keys().at(index.row());
        QVariant value = d.value(key);
        if (value.type() == QVariant::Color)
            return value;
    }

    return QVariant();
}

bool SettingsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() >= d.count())
        return false;

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QString key = d.keys().at(index.row());
        if (index.column() == 0) {
            // TODO
        } else {
            d.insert(key, value);
            emit dataChanged(index, index);
        }
        return true;
    }

    return false;
}
