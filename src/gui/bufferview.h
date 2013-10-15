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

#include <QUrl>
#include <QHash>
#include <QStackedWidget>

class IrcUser;
class IrcBuffer;
class IrcMessage;
class IrcPalette;
class TextBrowser;
class IrcUserModel;
class TextDocument;
class MessageFormatter;

class BufferView : public QStackedWidget
{
    Q_OBJECT

public:
    BufferView(QWidget* parent = 0);
    ~BufferView();

    IrcBuffer* currentBuffer() const;

    QWidget* buddy() const;
    void setBuddy(QWidget* buddy);

public slots:
    void addBuffer(IrcBuffer* buffer);
    void removeBuffer(IrcBuffer* buffer);
    void setCurrentBuffer(IrcBuffer* buffer);

signals:
    void clicked(const QString& user);
    void highlighted(IrcBuffer* buffer);
    void highlighted(IrcMessage* message);

private slots:
    void onAnchorClicked(const QUrl& url);
    void receiveMessage(IrcMessage* message);

private:
    struct Private {
        QString css;
        QWidget* bud;
        IrcBuffer* buffer;
        IrcUserModel* userModel;
        MessageFormatter* formatter;
        QHash<IrcBuffer*, QString> buffers;
        QHash<IrcBuffer*, TextBrowser*> browsers;
    } d;
};

#endif // BUFFERVIEW_H
