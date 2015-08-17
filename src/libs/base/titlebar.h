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

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QLabel>
#include <QTextEdit>
#include <QToolButton>

class IrcBuffer;
class IrcUserModel;
class MessageFormatter;

class TitleBar : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)

public:
    explicit TitleBar(QWidget* parent = 0);

    IrcBuffer* buffer() const;
    QString topic() const;

    QMenu* menu() const;

    QSize minimumSizeHint() const;
    int heightForWidth(int width) const;

    int offset() const;
    int baseOffset() const;

    QString styleSheet() const;
    void setStyleSheet(const QString& css);

public slots:
    void setBuffer(IrcBuffer* buffer);
    void setTopic(const QString& topic);

    void expand();
    void collapse();
    void setOffset(int offset);

signals:
    void offsetChanged(int offset);
    void bufferChanged(IrcBuffer* buffer);

protected:
    bool event(QEvent* event);
    bool eventFilter(QObject* object, QEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);
    void resizeEvent(QResizeEvent* event);

private slots:
    void relayout();
    void cleanup();
    void refresh();
    void edit();

private:
    int topMargin() const;

    struct Private {
        QString css;
        int baseOffset;
        IrcBuffer* buffer;
        QTextEdit* editor;
        QToolButton* menuButton;
        MessageFormatter* formatter;
        IrcUserModel* model;
    } d;
};

#endif // TITLEBAR_H
