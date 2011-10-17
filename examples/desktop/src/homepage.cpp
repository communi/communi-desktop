/*
* Copyright (C) 2008-2011 J-P Nurmi jpnurmi@gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "homepage.h"
#include "application.h"
#include <QCommandLinkButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QMenu>
#include <QFile>

HomePage::HomePage(QWidget* parent) : QWidget(parent)
{
    bg.load(":/resources/background.png");

    header = new QLabel(this);
    header->setOpenExternalLinks(true);
    header->setWordWrap(true);
    footer = new QLabel(this);
    footer->setOpenExternalLinks(true);

    updateHtml();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(header);
    layout->addWidget(createBody(this), 1);
    layout->addWidget(footer);
    setLayout(layout);
}

void HomePage::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.drawPixmap(width() - bg.width(), height() - bg.height(), bg);
}

static QString readHtmlFile(const QString& filePath)
{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    return QString::fromUtf8(file.readAll());
}

void HomePage::updateHtml()
{
    QString headerHtml = readHtmlFile(":/resources/welcome_header.html");
    headerHtml = headerHtml.arg(Application::applicationName())
                           .arg(Application::applicationSlogan());
    header->setText(headerHtml);
    footer->setText(readHtmlFile(":/resources/welcome_footer.html"));
}

QWidget* HomePage::createBody(QWidget* parent) const
{
    QWidget* body = new QWidget(parent);

    QCommandLinkButton* connectButton = new QCommandLinkButton(tr("Connect"), body);
    connectButton->setDescription(tr("New IRC connection"));
    QCommandLinkButton* settingsButton = new QCommandLinkButton(tr("Settings"), body);
    settingsButton->setDescription(tr("Configure %1").arg(Application::applicationName()));

    connect(connectButton, SIGNAL(clicked()), this, SIGNAL(connectRequested()));
    connect(settingsButton, SIGNAL(clicked()), qApp, SLOT(showSettings()));

    QGridLayout* layout = new QGridLayout(body);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(3, 1);
    layout->setRowStretch(0, 1);
    layout->addWidget(connectButton, 1, 1);
    layout->addWidget(settingsButton, 1, 2);
    layout->setRowStretch(7, 1);
    body->setLayout(layout);

    return body;
}
