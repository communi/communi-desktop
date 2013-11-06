/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef BUFFERVIEW_H
#define BUFFERVIEW_H

#include <QSplitter>

class ListView;
class IrcBuffer;
class TextInput;
class TopicLabel;
class TextBrowser;

class BufferView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* buffer READ buffer WRITE setBuffer NOTIFY bufferChanged)

public:
    explicit BufferView(QWidget* parent = 0);
    ~BufferView();

    IrcBuffer* buffer() const;

    ListView* listView() const;
    TextInput* textInput() const;
    TopicLabel* topicLabel() const;
    TextBrowser* textBrowser() const;

public slots:
    void setBuffer(IrcBuffer* buffer);

signals:
    void destroyed(BufferView* view);
    void bufferChanged(IrcBuffer* buffer);

private slots:
    void query(const QString& user);

private:
    struct Private {
        IrcBuffer* buffer;
        ListView* listView;
        TextInput* textInput;
        TopicLabel* topicLabel;
        TextBrowser* textBrowser;
        QSplitter* splitter;
    } d;
};

#endif // BUFFERVIEW_H
