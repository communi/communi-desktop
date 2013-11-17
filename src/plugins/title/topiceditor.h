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

#ifndef TOPICEDITOR_H
#define TOPICEDITOR_H

#include <QTextEdit>

class TitleBar;

class TopicEditor : public QTextEdit
{
    Q_OBJECT

public:
    TopicEditor(TitleBar* parent = 0);

    bool eventFilter(QObject* object, QEvent* event);

public slots:
    void edit();

protected:
    bool event(QEvent* event);

private:
    struct Private {
        TitleBar* bar;
    } d;
};

#endif // TOPICEDITOR_H
