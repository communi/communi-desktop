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

#include "themewidget.h"
#include "textbrowser.h"
#include "treewidget.h"
#include "bufferview.h"
#include "splitview.h"
#include "chatpage.h"
#include <IrcBufferModel>
#include <IrcConnection>
#include <IrcChannel>
#include <IrcMessage>
#include <IrcBuffer>
#include <QPainter>
#include <QPixmap>
#include <QTimer>

class Channel : public IrcChannel
{
public:
    Channel(QObject* parent = 0) : IrcChannel(parent) { }
    bool isActive() const { return true; }
};

class Server : public IrcBuffer
{
public:
    Server(QObject* parent = 0) : IrcBuffer(parent) { }
    bool isActive() const { return true; }
};

static void receiveMessages(IrcBufferModel* model, IrcBuffer* buffer)
{
    QString title = buffer->title();
    IrcConnection* connection = buffer->connection();
    model->receiveMessage(IrcMessage::fromParameters("Lorem", "JOIN", QStringList() << title, connection));
    IrcNamesMessage* names = new IrcNamesMessage(connection);
    names->setParameters(QStringList() << title << "Lorem" << "Morbi" << "nulla" << "rhoncus" << "Etiam" << "nunc" << "gravida" << "Proin" << "Cras");
    model->receiveMessage(names);
    model->receiveMessage(IrcMessage::fromParameters("Morbi", "PRIVMSG", QStringList() << title << "nullam eget commodo diam. sit amet ornare leo.", connection));
    model->receiveMessage(IrcMessage::fromParameters("nulla", "PRIVMSG", QStringList() << title << "...", connection));
    model->receiveMessage(IrcMessage::fromParameters("Ipsum", "JOIN", QStringList() << title, connection));
    model->receiveMessage(IrcMessage::fromParameters("rhoncus", "PRIVMSG", QStringList() << title << "vivamus!", connection));
    model->receiveMessage(IrcMessage::fromParameters("Etiam", "PRIVMSG", QStringList() << title << "Ut volutpat nibh nec enim elementum, sed placerat erat gravida.", connection));
    model->receiveMessage(IrcMessage::fromParameters("Ipsum", "QUIT", QStringList() << "Nulla facilisi.", connection));
    model->receiveMessage(IrcMessage::fromParameters("Ipsum", "JOIN", QStringList() << title, connection));
    model->receiveMessage(IrcMessage::fromParameters("nunc", "PRIVMSG", QStringList() << title << ":)", connection));
    model->receiveMessage(IrcMessage::fromParameters("gravida", "PRIVMSG", QStringList() << title << "etiam augue purus, pharetra vel neque a, fringilla hendrerit orci", connection));
    model->receiveMessage(IrcMessage::fromParameters("Proin", "PRIVMSG", QStringList() << title << "enim ante?", connection));
    model->receiveMessage(IrcMessage::fromParameters("Cras", "PRIVMSG", QStringList() << title << "quisque vel lacus eu odio pretium adipiscing...", connection));
}

static IrcBuffer* createChannel(const QString& name, QObject* parent)
{
    IrcChannel* channel = new Channel(parent);
    channel->setPrefix("#");
    channel->setName(name);
    return channel;
}

static IrcBufferModel* createBufferModel(QObject* parent)
{
    IrcConnection* connection = new IrcConnection(parent);
    connection->setNickName("communi");
    IrcBufferModel* model = new IrcBufferModel(connection);
    IrcBuffer* server = new Server(model);
    server->setName("Lorem Ipsum");
    server->setSticky(true);
    model->add(server);
    model->add(createChannel("donec", model));
    model->add(createChannel("convallis", model));
    model->add(createChannel("sagittis", model));
    model->add(createChannel("magna", model));
    model->add(createChannel("tincidunt", model));
    model->add(createChannel("phasellus ", model));
    model->add(createChannel("tellus", model));
    model->add(createChannel("fermentum", model));
    model->add(createChannel("pharetra", model));
    model->add(createChannel("vehicula", model));
    model->add(createChannel("aliquam", model));
    model->add(createChannel("bibendum", model));
    model->add(createChannel("semper", model));
    model->add(createChannel("dictum", model));
    model->add(createChannel("rhoncus", model));
    return model;
}

static ChatPage* createChatPage(QWidget* parent = 0)
{
    ChatPage* page = new ChatPage(parent);
    IrcBufferModel* model = createBufferModel(page);
    foreach (IrcBuffer* buffer, model->buffers())
        page->treeWidget()->addBuffer(buffer);
    IrcBuffer* buffer = model->find("#magna");
    page->addBuffer(buffer);
    page->splitView()->setCurrentBuffer(buffer);
    page->treeWidget()->setCurrentBuffer(buffer);
    page->treeWidget()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    page->currentView()->textBrowser()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    receiveMessages(model, buffer);
    return page;
}

ThemeWidget::ThemeWidget(QWidget* parent) : QLabel(parent)
{
    d.page = createChatPage();
    d.page->setVisible(false);
    d.page->resize(960, 600);

    QTimer::singleShot(0, this, SLOT(updatePreview()));
}

ThemeWidget::~ThemeWidget()
{
    delete d.page;
}

QString ThemeWidget::theme() const
{
    return d.page->theme();
}

void ThemeWidget::setTheme(const QString& theme)
{
    if (theme != d.page->theme()) {
        d.page->setTheme(theme);
        updatePreview();
    }
}

void ThemeWidget::resizeEvent(QResizeEvent* event)
{
    QLabel::resizeEvent(event);
    updatePreview();
}

void ThemeWidget::updatePreview()
{
    QPixmap pixmap(d.page->size());
    QPainter painter(&pixmap);
    d.page->render(&painter);
    setPixmap(pixmap.scaled(contentsRect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
