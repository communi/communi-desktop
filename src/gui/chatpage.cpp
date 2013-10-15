/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "chatpage.h"
#include "treeview.h"
#include "bufferview.h"
#include "topiclabel.h"
#include "userlistview.h"
#include "messageformatter.h"
#include "searchentry.h"
#include "searchpopup.h"
#include "textentry.h"
#include <IrcBufferModel>
#include <IrcConnection>
#include <QApplication>
#include <QVBoxLayout>
#include <IrcChannel>
#include <QShortcut>
#include <QSettings>
#include <IrcUser>

ChatPage::ChatPage(QWidget* parent) : QWidget(parent)
{
    d.treeView = new TreeView(this);
    connect(d.treeView, SIGNAL(currentBufferChanged(IrcBuffer*)), this, SLOT(setCurrentBuffer(IrcBuffer*)));

    d.userListView = new UserListView(this);
    connect(d.userListView, SIGNAL(doubleClicked(QString)), this, SLOT(queryUser(QString)));

    d.textEntry = new TextEntry(this);
    d.topicLabel = new TopicLabel(this);
    d.bufferView = new BufferView(this);
    d.bufferView->setBuddy(d.textEntry);

    connect(d.bufferView, SIGNAL(clicked(QString)), this, SLOT(queryUser(QString)));
    connect(d.bufferView, SIGNAL(highlighted(IrcBuffer*)), d.treeView, SLOT(highlight(IrcBuffer*)));
    connect(d.bufferView, SIGNAL(highlighted(IrcMessage*)), this, SIGNAL(highlighted(IrcMessage*)));

    d.searchEntry = new SearchEntry(this);

    QShortcut* shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onEscPressed()));

    shortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(searchBuffer()));

    QWidget* container = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(d.topicLabel);
    layout->addWidget(d.bufferView);
    layout->addWidget(d.searchEntry);

    d.innerSplitter = new QSplitter(this);
    d.innerSplitter->setHandleWidth(1);
    d.innerSplitter->addWidget(container);
    d.innerSplitter->addWidget(d.userListView);
    d.innerSplitter->setStretchFactor(0, 1);

    container = new QWidget(this);
    layout = new QVBoxLayout(container);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(d.innerSplitter);
    layout->addWidget(d.textEntry);

    d.outerSplitter = new QSplitter(this);
    d.outerSplitter->addWidget(d.treeView);
    d.outerSplitter->addWidget(container);
    d.outerSplitter->setStretchFactor(1, 1);
    d.outerSplitter->setHandleWidth(1);

    layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(d.outerSplitter);

    setCurrentBuffer(0);

//    QSettings settings;
//    settings.beginGroup("Widgets/ChatPage");
//    if (settings.contains("geometry"))
//        d.outerSplitter->restoreState(settings.value("geometry").toByteArray());
//    if (settings.contains("state"))
//        d.innerSplitter->restoreState(settings.value("state").toByteArray());
}

ChatPage::~ChatPage()
{
    QSettings settings;
    settings.beginGroup("Widgets/ChatPage");
    settings.setValue("geometry", d.outerSplitter->saveState());
    settings.setValue("state", d.innerSplitter->saveState());
}

IrcBuffer* ChatPage::currentBuffer() const
{
    return d.bufferView->currentBuffer();
}

QList<IrcConnection*> ChatPage::connections() const
{
    return d.connections;
}

void ChatPage::addConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = new IrcBufferModel(connection);

    IrcBuffer* serverBuffer = bufferModel->add(connection->displayName());
    connect(connection, SIGNAL(displayNameChanged(QString)), serverBuffer, SLOT(setName(QString)));
    connect(bufferModel, SIGNAL(messageIgnored(IrcMessage*)), serverBuffer, SLOT(receiveMessage(IrcMessage*)));

    d.bufferView->addBuffer(serverBuffer);
    connect(bufferModel, SIGNAL(added(IrcBuffer*)), d.bufferView, SLOT(addBuffer(IrcBuffer*)));
    connect(bufferModel, SIGNAL(removed(IrcBuffer*)), d.bufferView, SLOT(removeBuffer(IrcBuffer*)));

    d.treeView->addModel(bufferModel);
    if (!d.treeView->currentBuffer())
        d.treeView->setCurrentBuffer(serverBuffer);

    d.connections += connection;
}

void ChatPage::removeConnection(IrcConnection* connection)
{
    IrcBufferModel* bufferModel = connection->findChild<IrcBufferModel*>();
    disconnect(bufferModel, SIGNAL(added(IrcBuffer*)), d.bufferView, SLOT(addBuffer(IrcBuffer*)));
    disconnect(bufferModel, SIGNAL(removed(IrcBuffer*)), d.bufferView, SLOT(removeBuffer(IrcBuffer*)));
    disconnect(bufferModel, SIGNAL(added(IrcBuffer*)), d.textEntry, SLOT(addBuffer(IrcBuffer*)));
    disconnect(bufferModel, SIGNAL(removed(IrcBuffer*)), d.textEntry, SLOT(removeBuffer(IrcBuffer*)));

    d.treeView->removeModel(bufferModel);
    d.connections.removeOne(connection);
}

void ChatPage::searchBuffer()
{
    SearchPopup* search = new SearchPopup(this);
    connect(search, SIGNAL(searched(QString)), d.treeView, SLOT(searchItem(QString)));
    connect(search, SIGNAL(searchedAgain(QString)), d.treeView, SLOT(searchAgain(QString)));
    connect(search, SIGNAL(destroyed()), d.treeView, SLOT(unblockItemReset()));
    connect(d.treeView, SIGNAL(searched(bool)), search, SLOT(onSearched(bool)));
    d.treeView->blockItemReset();
    search->popup();
}

void ChatPage::onEscPressed()
{
    d.searchEntry->hide();
    d.textEntry->setFocus(Qt::OtherFocusReason);
}

void ChatPage::queryUser(const QString& user)
{
    if (!user.isEmpty()) {
        IrcBuffer* buffer = d.bufferView->currentBuffer();
        IrcBufferModel* model = buffer->model();
        IrcBuffer* newBuffer = model->add(user);
        d.treeView->setCurrentBuffer(newBuffer);
    }
}

void ChatPage::setCurrentBuffer(IrcBuffer* buffer)
{
    d.bufferView->setCurrentBuffer(buffer);
    //d.treeView->setVisible(buffer);
    d.textEntry->setCurrentBuffer(buffer);
    //d.textEntry->setVisible(buffer);
    if (buffer)
        d.textEntry->setFocus();
    d.searchEntry->setTextEdit(qobject_cast<QTextEdit*>(d.bufferView->currentWidget()));

    IrcChannel* channel = qobject_cast<IrcChannel*>(buffer);
    d.topicLabel->setChannel(channel);
    d.topicLabel->setVisible(channel);
    d.userListView->setChannel(channel);
    d.userListView->setVisible(channel);

    emit currentBufferChanged(buffer);
}
