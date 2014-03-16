/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "settingspage.h"
#include "textdocument.h"
#include "textbrowser.h"
#include "themeloader.h"
#include "treewidget.h"
#include "bufferview.h"
#include "themeinfo.h"
#include "splitview.h"
#include "chatpage.h"
#include <IrcBufferModel>
#include <IrcConnection>
#include <QPushButton>
#include <IrcChannel>
#include <IrcMessage>
#include <QShortcut>
#include <IrcBuffer>
#include <QPainter>
#include <QPixmap>
#include <QBuffer>

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

static void receiveMessages(TextDocument* doc)
{
    QString title = doc->buffer()->title();
    IrcConnection* connection = doc->buffer()->connection();
    doc->receiveMessage(IrcMessage::fromParameters("Lorem", "JOIN", QStringList() << title, connection));
    doc->receiveMessage(IrcMessage::fromParameters("Morbi", "PRIVMSG", QStringList() << title << "nullam eget commodo diam. sit amet ornare leo.", connection));
    doc->receiveMessage(IrcMessage::fromParameters("nulla", "PRIVMSG", QStringList() << title << "...", connection));
    doc->receiveMessage(IrcMessage::fromParameters("Ipsum", "JOIN", QStringList() << title, connection));
    doc->receiveMessage(IrcMessage::fromParameters("rhoncus", "PRIVMSG", QStringList() << title << "vivamus!", connection));
    doc->receiveMessage(IrcMessage::fromParameters("Etiam", "PRIVMSG", QStringList() << title << "Ut volutpat nibh nec enim elementum, sed placerat erat gravida.", connection));
    doc->receiveMessage(IrcMessage::fromParameters("Ipsum", "QUIT", QStringList() << "Nulla facilisi.", connection));
    doc->receiveMessage(IrcMessage::fromParameters("Ipsum", "JOIN", QStringList() << title, connection));
    doc->receiveMessage(IrcMessage::fromParameters("nunc", "PRIVMSG", QStringList() << title << ":)", connection));
    doc->receiveMessage(IrcMessage::fromParameters("gravida", "PRIVMSG", QStringList() << title << "etiam augue purus, pharetra vel neque a, fringilla hendrerit orci", connection));
    doc->receiveMessage(IrcMessage::fromParameters("Proin", "PRIVMSG", QStringList() << title << "enim ante?", connection));
    doc->receiveMessage(IrcMessage::fromParameters("Cras", "PRIVMSG", QStringList() << title << "quisque vel lacus eu odio pretium adipiscing...", connection));
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
    receiveMessages(page->currentView()->textDocument());
    return page;
}

SettingsPage::SettingsPage(QWidget* parent) : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.buttonBox, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SIGNAL(rejected()));

    QShortcut* shortcut = new QShortcut(Qt::Key_Return, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Ok), SLOT(click()));

    shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Cancel), SLOT(click()));

    ChatPage* page = createChatPage();
    page->resize(640, 400);

    foreach (const QString& name, ThemeLoader::instance()->themes()) {
        ThemeInfo theme = ThemeLoader::instance()->theme(name);

        QPixmap pixmap(640, 400);
        QPainter painter(&pixmap);

        page->setTheme(theme.name());
        page->render(&painter);

        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        pixmap.scaled(320, 200).save(&buffer, "PNG");

        QLabel* label = new QLabel(ui.content);
        label->setTextFormat(Qt::RichText);
        label->setText(tr("<table><tr>"
                          "<td>"
                          "<img src='data:image/png;base64,%5'/>"
                          "</td>"
                          "<td>"
                          "<h1>%1</h1>"
                          "<p><b>Version</b>: %2</p>"
                          "<p><b>Author</b>: %3</p>"
                          "<p>%4</p>"
                          "</td>"
                          "</tr></table>").arg(theme.name(), theme.version(), theme.author(), theme.description(), ba.toBase64()));
        ui.content->layout()->addWidget(label);
    }

    delete page;
}

SettingsPage::~SettingsPage()
{
}

QString SettingsPage::theme() const
{
    //return ui.comboBox->currentText();
    return "Cute";
}
/*
void SettingsPage::setThemes(const QStringList& themes)
{
    //ui.comboBox->clear();
    //ui.comboBox->addItems(themes);
}
*/

