/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#include "welcomepage.h"
#include "application.h"
#include <QCommandLinkButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QFile>

WelcomePage::WelcomePage(QWidget* parent) : QWidget(parent)
{
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

static QString readHtmlFile(const QString& filePath)
{
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    return QString::fromUtf8(file.readAll());
}

void WelcomePage::updateHtml()
{
    QString headerHtml = readHtmlFile(":/resources/welcome_header.html");
    headerHtml = headerHtml.arg(Application::applicationName())
                           .arg(Application::applicationSlogan());
    header->setText(headerHtml);
    footer->setText(readHtmlFile(":/resources/welcome_footer.html"));
}

QWidget* WelcomePage::createBody(QWidget* parent) const
{
    QWidget* body = new QWidget(parent);

#ifdef Q_WS_MAEMO_5
    QPushButton* connectButton = new QPushButton(QString("%1\n%2").arg(tr("Connect"), tr("New IRC connection")), body);
    QPushButton* settingsButton = new QPushButton(QString("%1\n%2").arg(tr("Settings"), tr("Configure %1").arg(Application::applicationName())), body);
#else
    QCommandLinkButton* connectButton = new QCommandLinkButton(tr("Connect"), body);
    connectButton->setDescription(tr("New IRC connection"));
    QCommandLinkButton* settingsButton = new QCommandLinkButton(tr("Settings"), body);
    settingsButton->setDescription(tr("Configure %1").arg(Application::applicationName()));
#endif

    connect(connectButton, SIGNAL(clicked()), this, SIGNAL(connectRequested()));
    connect(settingsButton, SIGNAL(clicked()), qApp, SLOT(showSettings()));

#if 0 // ifndef Q_WS_MAEMO_5
    QCommandLinkButton* aboutAppButton = new QCommandLinkButton(Application::applicationName(), body);
    aboutAppButton->setDescription(tr("About %1").arg(Application::applicationName()));
    connect(aboutAppButton, SIGNAL(clicked()), qApp, SLOT(aboutApplication()));

    QCommandLinkButton* aboutOxygenButton = new QCommandLinkButton(tr("Oxygen"), body);
    aboutOxygenButton->setDescription(tr("About Oxygen icons"));
    connect(aboutOxygenButton, SIGNAL(clicked()), qApp, SLOT(aboutOxygen()));

    QCommandLinkButton* aboutQtButton = new QCommandLinkButton(tr("Qt"), body);
    aboutQtButton->setDescription(tr("About Qt toolkit"));
    connect(aboutQtButton, SIGNAL(clicked()), qApp, SLOT(aboutQt()));
#endif // !Q_WS_MAEMO_5

    QGridLayout* layout = new QGridLayout(body);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(3, 1);
    layout->setRowStretch(0, 1);
    layout->addWidget(connectButton, 1, 1);
    layout->addWidget(settingsButton, 1, 2);
#if 0 // ifndef Q_WS_MAEMO_5
    layout->addWidget(aboutAppButton, 3, 1);
    layout->addWidget(aboutOxygenButton, 4, 1);
    layout->addWidget(aboutQtButton, 5, 1);
    layout->setRowStretch(6, 1);
#endif // !Q_WS_MAEMO_5
    layout->setRowStretch(7, 1);
    body->setLayout(layout);

    return body;
}
