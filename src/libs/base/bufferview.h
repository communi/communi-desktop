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
    TextDocument* textDocument() const;

public slots:
    void setBuffer(IrcBuffer* buffer);
    void closeBuffer();

signals:
    void destroyed(BufferView* view);
    void bufferChanged(IrcBuffer* buffer);
    void bufferClosed(IrcBuffer* buffer);
    void cloned(TextDocument* doc);

protected:
    void resizeEvent(QResizeEvent* event);

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

Q_DECLARE_METATYPE(BufferView*)

#endif // BUFFERVIEW_H
