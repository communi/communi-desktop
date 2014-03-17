/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QSplitter>
#include <IrcCommandFilter>
#include "themeinfo.h"

class IrcBuffer;
class SplitView;
class TreeWidget;
class BufferView;
class TextDocument;
class IrcConnection;
class IrcCommandParser;

class ChatPage : public QSplitter, public IrcCommandFilter
{
    Q_OBJECT
    Q_INTERFACES(IrcCommandFilter)

public:
    ChatPage(QWidget* parent = 0);
    ~ChatPage();

    void init();

    QString theme() const;
    void setTheme(const QString& theme);

    QFont messageFont() const;
    void setMessageFont(const QFont& font);

    QString timeStampFormat() const;
    void setTimeStampFormat(const QString& format);

    bool showEvents() const;
    void setShowEvents(bool show);

    bool showDetails() const;
    void setShowDetails(bool show);

    TreeWidget* treeWidget() const;
    SplitView* splitView() const;

    BufferView* currentView() const;
    IrcBuffer* currentBuffer() const;

    QByteArray saveSettings() const;
    void restoreSettings(const QByteArray& data);

    QByteArray saveState() const;
    void restoreState(const QByteArray& data);

public slots:
    void addBuffer(IrcBuffer* buffer);
    void closeBuffer(IrcBuffer* buffer = 0);
    void removeBuffer(IrcBuffer* buffer);
    void setupDocument(TextDocument* document);

signals:
    void currentViewChanged(BufferView* view);
    void currentBufferChanged(IrcBuffer* buffer);

protected:
    bool commandFilter(IrcCommand* command);

private slots:
    void addConnection(IrcConnection* connection);
    void removeConnection(IrcConnection* connection);
    void addView(BufferView* view);
    void removeView(BufferView* view);
    void onSocketError();

private:
    static IrcCommandParser* createParser(QObject* parent);

    struct Private {
        bool showEvents;
        bool showDetails;
        QFont messageFont;
        QString timestampFormat;
        ThemeInfo theme;
        QStringList chans;
        SplitView* splitView;
        TreeWidget* treeWidget;
    } d;
};

#endif // CHATPAGE_H
