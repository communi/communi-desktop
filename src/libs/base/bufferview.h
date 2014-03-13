/*
* Copyright (C) 2008-2014 The Communi Project
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

#ifndef BUFFERVIEW_H
#define BUFFERVIEW_H

#include <QSplitter>

class TitleBar;
class ListView;
class IrcBuffer;
class TextInput;
class TextBrowser;
class TextDocument;

class BufferView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* buffer READ buffer WRITE setBuffer NOTIFY bufferChanged)

public:
    explicit BufferView(QWidget* parent = 0);
    ~BufferView();

    IrcBuffer* buffer() const;

    TitleBar* titleBar() const;
    ListView* listView() const;
    TextInput* textInput() const;
    TextBrowser* textBrowser() const;

public slots:
    void setBuffer(IrcBuffer* buffer);
    void closeBuffer();

signals:
    void destroyed(BufferView* view);
    void bufferChanged(IrcBuffer* buffer);
    void bufferClosed(IrcBuffer* buffer);
    void cloned(TextDocument* doc);

private slots:
    void query(const QString& user);

private:
    struct Private {
        IrcBuffer* buffer;
        TitleBar* titleBar;
        ListView* listView;
        TextInput* textInput;
        TextBrowser* textBrowser;
        QSplitter* splitter;
    } d;
};

#endif // BUFFERVIEW_H
