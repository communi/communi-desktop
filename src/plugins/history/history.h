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

#ifndef HISTORY_H
#define HISTORY_H

#include <QHash>
#include <QObject>
#include <QStringList>

class IrcBuffer;
class TextInput;

class History : public QObject
{
    Q_OBJECT

public:
    History(TextInput* input);

    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void goBackward();
    void goForward();
    void changeBuffer(IrcBuffer* buffer);

private:
    struct Private {
        int index;
        QString text;
        TextInput* input;
        IrcBuffer* buffer;
        QStringList history;
        QHash<IrcBuffer*, QStringList> histories;
    } d;
};

#endif // HISTORY_H
