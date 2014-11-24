/*
  Copyright (C) 2008-2014 The Communi Project

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

#include "connectpage.h"
#include "simplecrypt.h"
#include "scrollbarstyle.h"
#include <QRegExpValidator>
#include <IrcConnection>
#include <QStringList>
#include <QPushButton>
#include <QScrollBar>
#include <QCompleter>
#include <QSettings>
#include <QShortcut>
#include <QRegExp>
#include <QTime>
#include <Irc>

static QStringList splitLines(const QString& nicks)
{
    return nicks.split("\n", QString::SkipEmptyParts);
}

ConnectPage::ConnectPage(QWidget* parent) : QWidget(parent)
{
    init();
}

ConnectPage::ConnectPage(IrcConnection* connection, QWidget* parent) : QWidget(parent)
{
    init(connection);
}

ConnectPage::~ConnectPage()
{
}

static QString fieldValue(const QString& value, const QString& fallback)
{
    return value.isEmpty() ? fallback : value;
}

QString ConnectPage::displayName() const
{
    return fieldValue(ui.displayNameField->text(), ui.displayNameField->placeholderText());
}

void ConnectPage::setDisplayName(const QString& name)
{
    ui.displayNameField->setText(name);
}

QStringList ConnectPage::servers() const
{
    QStringList lines = splitLines(ui.serverField->toPlainText());
    if (lines.isEmpty())
        lines += ui.serverField->placeholderText();
    return lines;
}

void ConnectPage::setServers(const QStringList& servers)
{
    ui.serverField->clear();
    foreach (const QString& line, servers)
        ui.serverField->appendPlainText(line);
}

QString ConnectPage::saslMechanism() const
{
    return ui.saslBox->isChecked() ? IrcConnection::supportedSaslMechanisms().first() : QString();
}

void ConnectPage::setSaslMechanism(const QString& mechanism)
{
    ui.saslBox->setChecked(!mechanism.isEmpty());
}

QStringList ConnectPage::nickNames() const
{
    QStringList nicks = splitLines(ui.nickNameField->toPlainText());
    if (nicks.isEmpty())
        nicks += ui.nickNameField->placeholderText();
    return nicks;
}

void ConnectPage::setNickNames(const QStringList& names)
{
    ui.nickNameField->clear();
    foreach (const QString& name, names)
        ui.nickNameField->appendPlainText(name);
}

QString ConnectPage::realName() const
{
    return fieldValue(ui.realNameField->text(), ui.realNameField->placeholderText());
}

void ConnectPage::setRealName(const QString& name)
{
    ui.realNameField->setText(name);
}

QString ConnectPage::userName() const
{
    return fieldValue(ui.userNameField->text(), ui.userNameField->placeholderText());
}

void ConnectPage::setUserName(const QString& name)
{
    ui.userNameField->setText(name);
}

QString ConnectPage::password() const
{
    return ui.passwordField->text();
}

void ConnectPage::setPassword(const QString& password)
{
    ui.passwordField->setText(password);
}

IrcConnection* ConnectPage::connection() const
{
    return ui.connection;
}

QDialogButtonBox* ConnectPage::buttonBox() const
{
    return ui.buttonBox;
}

void ConnectPage::autoFill()
{
    QString displayName = ui.displayNameField->text();
    if (!displayName.isEmpty()) {
        if (ui.serverField->toPlainText().isEmpty())
            setServers(splitLines(defaultValue("servers", displayName).toString()));
        if (ui.nickNameField->toPlainText().isEmpty())
            setNickNames(splitLines(defaultValue("nickNames", displayName).toString()));
        if (ui.realNameField->text().isEmpty())
            setRealName(defaultValue("realNames", displayName).toString());
        if (ui.userNameField->text().isEmpty())
            setUserName(defaultValue("userNames", displayName).toString());
    }
}

static QCompleter* createCompleter(const QStringList& list, QLineEdit* lineEdit)
{
    QCompleter* completer = new QCompleter(list, lineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    lineEdit->setCompleter(completer);
    return completer;
}

void ConnectPage::restoreSettings()
{
    QSettings settings;
    QVariantMap credentials = settings.value("credentials").toMap();

    ui.displayNameField->setText(credentials.value("displayName").toString());
    ui.serverField->setPlainText(credentials.value("servers").toString());
    ui.nickNameField->setPlainText(credentials.value("nickNames").toString());
    ui.realNameField->setText(credentials.value("realName").toString());
    ui.userNameField->setText(credentials.value("userName").toString());

    SimpleCrypt crypto(Q_UINT64_C(0x600af3d6a24df27c));
    ui.passwordField->setText(crypto.decryptToString(credentials.value("password").toString()));

    ui.displayNameCompleter = createCompleter(credentials.value("displayNames").toStringList(), ui.displayNameField);
    ui.realNameCompleter = createCompleter(credentials.value("allRealNames").toStringList(), ui.realNameField);
    ui.userNameCompleter = createCompleter(credentials.value("allUserNames").toStringList(), ui.userNameField);
}

void ConnectPage::saveSettings()
{
    const QString displayName = ui.displayNameField->text();
    const QString servers = ui.serverField->toPlainText();
    const QString nickNames = ui.nickNameField->toPlainText();
    const QString realName = ui.realNameField->text();
    const QString userName = ui.userNameField->text();

    QSettings settings;
    QVariantMap credentials = settings.value("credentials").toMap();
    credentials.insert("displayName", displayName);
    credentials.insert("servers", servers);
    credentials.insert("nickNames", nickNames);
    credentials.insert("realName", realName);
    credentials.insert("userName", userName);

    SimpleCrypt crypto(Q_UINT64_C(0x600af3d6a24df27c));
    credentials.insert("password", crypto.encryptToString(ui.passwordField->text()));

    if (!displayName.isEmpty()) {
        QStringList displayNames = credentials.value("displayNames").toStringList();
        if (!displayNames.contains(displayName, Qt::CaseInsensitive))
            credentials.insert("displayNames", displayNames << displayName);
    }

    if (!servers.isEmpty()) {
        QMap<QString, QVariant> servers = credentials.value("servers").toMap();
        servers.insert(ConnectPage::displayName(), servers);
        credentials.insert("servers", servers);
    }

    if (!nickNames.isEmpty()) {
        QMap<QString, QVariant> nickNames = credentials.value("nickNames").toMap();
        nickNames.insert(ConnectPage::displayName(), nickNames);
        credentials.insert("nickNames", nickNames);
    }

    if (!realName.isEmpty()) {
        QStringList allRealNames = credentials.value("allRealNames").toStringList();
        if (!allRealNames.contains(realName, Qt::CaseInsensitive))
            credentials.insert("allRealNames", allRealNames << realName);

        QMap<QString, QVariant> realNames = credentials.value("realNames").toMap();
        realNames.insert(ConnectPage::displayName(), realName);
        credentials.insert("realNames", realNames);
    }

    if (!userName.isEmpty()) {
        QStringList allUserNames = credentials.value("allUserNames").toStringList();
        if (!allUserNames.contains(userName, Qt::CaseInsensitive))
            credentials.insert("allUserNames", allUserNames << userName);

        QMap<QString, QVariant> userNames = credentials.value("userNames").toMap();
        userNames.insert(ConnectPage::displayName(), userName);
        credentials.insert("userNames", userNames);
    }

    settings.setValue("credentials", credentials);
}

void ConnectPage::updateUi()
{
    int lineSpacing = fontMetrics().lineSpacing();
    int margin = ui.serverField->document()->documentMargin();
    ui.serverField->setMinimumHeight(qMax(2, servers().count()) * lineSpacing + 2 * margin);
    ui.nickNameField->setMinimumHeight(qMax(2, nickNames().count()) * lineSpacing + 2 * margin);

    bool hasName = !ui.displayNameField->text().isEmpty();
    bool hasServer = !ui.serverField->toPlainText().isEmpty();
    bool hasNick = !ui.nickNameField->toPlainText().isEmpty();
    bool hasReal = !ui.realNameField->text().isEmpty();
    bool hasUser = !ui.userNameField->text().isEmpty();
    bool hasPass = !ui.passwordField->text().isEmpty();

    bool validServers = true;
    foreach (const QString& server, servers()) {
        if (!IrcConnection::isValidServer(server))
            validServers = false;
    }

    QPushButton* resetButton = ui.buttonBox->button(QDialogButtonBox::Reset);
    resetButton->setEnabled(hasName || hasServer || hasNick || hasReal || hasUser || hasPass);

    QPushButton* okButton = ui.buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(validServers);
}

void ConnectPage::reset()
{
    ui.displayNameField->clear();
    ui.serverField->clear();
    ui.nickNameField->clear();
    ui.realNameField->clear();
    ui.userNameField->clear();
    ui.passwordField->clear();
    saveSettings();
}

QVariant ConnectPage::defaultValue(const QString& key, const QString& field, const QVariant& defaultValue) const
{
    QSettings settings;
    QVariantMap credentials = settings.value("credentials").toMap();
    QMap<QString, QVariant> values = credentials.value(key).toMap();
    return values.value(field, defaultValue);
}

void ConnectPage::init(IrcConnection *connection)
{
    ui.setupUi(this);

    ui.scrollArea->horizontalScrollBar()->setStyle(ScrollBarStyle::narrow());
    ui.scrollArea->verticalScrollBar()->setStyle(ScrollBarStyle::narrow());

    ui.connection = connection;
    ui.displayNameCompleter = 0;
    ui.nickNameCompleter = 0;
    ui.realNameCompleter = 0;
    ui.userNameCompleter = 0;

    QRegExpValidator* validator = new QRegExpValidator(this);
    ui.userNameField->setValidator(validator);

    qsrand(QTime::currentTime().msec());
    ui.nickNameField->setPlaceholderText(ui.nickNameField->placeholderText().arg(qrand() % 9999));
    ui.realNameField->setPlaceholderText(ui.realNameField->placeholderText().arg(IRC_VERSION_STR));

    if (IrcConnection::isSecureSupported())
        ui.serverField->setPlaceholderText(ui.serverField->placeholderText().arg("+6697"));
    else
        ui.serverField->setPlaceholderText(ui.serverField->placeholderText().arg("6667"));

    connect(ui.buttonBox, SIGNAL(accepted()), ui.displayNameField, SLOT(setFocus()));
    connect(ui.buttonBox, SIGNAL(rejected()), ui.displayNameField, SLOT(setFocus()));

    connect(ui.buttonBox, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SIGNAL(rejected()));
    connect(ui.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));

    connect(ui.displayNameField, SIGNAL(textChanged(QString)), this, SLOT(autoFill()));

    connect(ui.displayNameField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.serverField, SIGNAL(textChanged()), this, SLOT(updateUi()));
    connect(ui.nickNameField, SIGNAL(textChanged()), this, SLOT(updateUi()));
    connect(ui.realNameField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.userNameField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.passwordField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));

    int labelWidth = 0;
    QList<QLabel*> labels;
    labels << ui.displayNameLabel << ui.serverLabel;
    labels << ui.nickNameLabel << ui.realNameLabel << ui.userNameLabel << ui.passwordLabel;
    foreach (QLabel* label, labels)
        labelWidth = qMax(labelWidth, label->sizeHint().width());
    foreach (QLabel* label, labels)
        label->setMinimumWidth(labelWidth);

    QShortcut* shortcut = new QShortcut(Qt::Key_Return, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Ok), SLOT(click()));

    shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Cancel), SLOT(click()));

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(reset()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(saveSettings()));
    restoreSettings();
    updateUi();
}
