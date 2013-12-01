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
#include <QPointer>

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

    static BufferView* current();
    static void setCurrent(BufferView* current);

    bool isCurrent() const;
    void setCurrent(bool current);

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
        static QPointer<BufferView> current;
    } d;
};

#endif // BUFFERVIEW_H
