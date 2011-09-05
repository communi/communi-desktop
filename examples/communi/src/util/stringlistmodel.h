/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
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

#ifndef STRINGLISTMODEL_H
#define STRINGLISTMODEL_H

#include <QStringListModel>

class StringListModel : public QStringListModel
{
    Q_OBJECT

public:
    StringListModel(QObject* parent = 0);

    QStringList stringList(int role) const;
    void setStringList(int role, const QStringList& stringList);

    void add(int role, const QString& str);
    void remove(int role, const QString& str);

private:
    struct StringListModelData
    {
        QMap<int, QStringList> lists;
    } d;
};

#endif // STRINGLISTMODEL_H
