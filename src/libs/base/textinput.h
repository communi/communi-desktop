/*
  Copyright (C) 2008-2016 The Communi Project

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

#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <QHash>
#include <QPointer>
#include <QLineEdit>
#include <QStringList>
#include <IrcCompleter>
#include "baseglobal.h"

class IrcBuffer;
class IrcCommandParser;

class BASE_EXPORT TextInput : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(IrcBuffer* buffer READ buffer WRITE setBuffer NOTIFY bufferChanged)
    Q_PROPERTY(IrcCommandParser* parser READ parser WRITE setParser NOTIFY parserChanged)

public:
    explicit TextInput(QWidget* parent = 0);

    IrcBuffer* buffer() const;
    IrcCommandParser* parser() const;

public slots:
    void setBuffer(IrcBuffer* buffer);
    void setParser(IrcCommandParser* parser);

signals:
    void bufferChanged(IrcBuffer* buffer);
    void parserChanged(IrcCommandParser* parser);

protected:
    bool event(QEvent* event);
    void paintEvent(QPaintEvent* event);

private slots:
    void updateHint(const QString& text);
    void goBackward();
    void goForward();
    void sendInput();
    void tryComplete(IrcCompleter::Direction direction);
    void doComplete(const QString& text, int cursor);

private:
    QByteArray saveState() const;
    void restoreState(const QByteArray& state);

    struct Private {
        int index;
        QString hint;
        QString current;
        QStringList history;
        IrcCompleter* completer;
        IrcCommandParser* parser;
        QPointer<IrcBuffer> buffer;
        QHash<IrcBuffer*, QByteArray> states;
    } d;
};

#endif // TEXTINPUT_H
