/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "settingspage.h"
#include "textdocument.h"
#include "themeloader.h"
#include "treewidget.h"
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

//"Nullam eget commodo diam, sit amet ornare leo."
//"Quisque vitae ipsum nisi."
//"Phasellus tristique augue a felis tincidunt, vel pretium odio ornare."
//"Mauris vel leo est."
//"Vivamus porttitor mauris eget bibendum consequat."
//"Ut volutpat nibh in enim elementum, sed placerat erat gravida."
//"Duis risus mauris, laoreet nec tempus in, varius sit amet est."
//"Etiam augue purus, pharetra vel neque a, fringilla hendrerit orci."

TextDocument* createTextDocument(IrcBuffer* buffer)
{
    TextDocument* doc = new TextDocument(buffer);
    doc->receiveMessage(IrcMessage::fromParameters("communi", "PRIVMSG", QStringList() << buffer->title() << "foo", buffer->connection()));
    return doc;
}

IrcBuffer* createChannel(const QString& name, QObject* parent)
{
    IrcChannel* channel = new IrcChannel(parent);
    channel->setPrefix("#");
    channel->setName(name);
    return channel;
}

static IrcBufferModel* createBufferModel(QObject* parent)
{
    IrcConnection* connection = new IrcConnection(parent);
    IrcBufferModel* model = new IrcBufferModel(connection);
    IrcBuffer* buffer = model->add("Lorem Ipsum");
    buffer->setSticky(true);
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
    createTextDocument(buffer);
    page->splitView()->setCurrentBuffer(buffer);
    page->treeWidget()->setCurrentBuffer(buffer);
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

