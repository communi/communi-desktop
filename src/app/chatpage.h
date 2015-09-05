/*
  Copyright (C) 2008-2015 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QSet>
#include <QSplitter>
#include <QDateTime>
#include <QVariantMap>
#include <IrcCommandFilter>
#include "themeinfo.h"

class Finder;
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

    QString theme() const;
    void setTheme(const QString& theme);

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

signals:
    void currentViewChanged(BufferView* view);
    void currentBufferChanged(IrcBuffer* buffer);

    void alert(IrcMessage* message);

protected:
    bool commandFilter(IrcCommand* command);

private slots:
    void addConnection(IrcConnection* connection);
    void removeConnection(IrcConnection* connection);
    void addView(BufferView* view);
    void removeView(BufferView* view);
    void setupDocument(TextDocument* document);
    void onCurrentBufferChanged(IrcBuffer* buffer);
    void onCurrentViewChanged(BufferView* current, BufferView* previous);
    void onMessageReceived(IrcMessage* message);
    void onAlert(IrcMessage* message);
    void onSocketError();
    void onSecureError();
    void onConnected();

private:
    static IrcCommandParser* createParser(QObject* parent);

    struct Private {
        Finder* finder;
        ThemeInfo theme;
        QString timestamp;
        QStringList chans;
        SplitView* splitView;
        TreeWidget* treeWidget;
        QVariantMap timestamps;
        IrcBuffer* currentBuffer;
        QSet<TextDocument*> documents;
    } d;
};

#endif // CHATPAGE_H
