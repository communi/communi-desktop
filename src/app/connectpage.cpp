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

#include "connectpage.h"
#include "simplecrypt.h"
#include "scrollbarstyle.h"
#include <QRegExpValidator>
#include <QStringListModel>
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

static const int SSL_PORTS[] = { 6697, 7000, 7070 };
static const int NORMAL_PORTS[] = { 6667, 6666, 6665 };

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
    // block autoFill()
    ui.displayNameField->blockSignals(true);
    ui.displayNameField->setText(name);
    ui.displayNameField->blockSignals(false);
}

QStringList ConnectPage::servers() const
{
    QString host = fieldValue(ui.hostField->text(), ui.hostField->placeholderText());
    QString port = QString::number(ui.portField->value());
    if (ui.secureBox->isChecked())
        port.prepend("+");

    QStringList servers = splitLines(ui.serverField->toPlainText());
    servers.prepend(host + " " + port);
    return servers;
}

void ConnectPage::setServers(const QStringList& servers)
{
    ui.hostField->clear();
    ui.portField->setValue(NORMAL_PORTS[0]);
    ui.secureBox->setChecked(false);
    ui.serverField->clear();

    for (int i = 0; i < servers.count(); ++i) {
        QString server = servers.at(i);
        if (i == 0) {
            QStringList values = server.split(" ", QString::SkipEmptyParts);
            ui.hostField->setText(values.value(0));
            if (values.count() > 1) {
                ui.portField->setValue(values.at(1).toInt());
                ui.secureBox->setChecked(values.at(1).startsWith("+"));
            }
        } else {
            ui.serverField->appendPlainText(server);
        }
    }
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
#if QT_VERSION >= 0x050300
    if (nicks.isEmpty())
        nicks += ui.nickNameField->placeholderText();
#endif // QT_VERSION
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

bool ConnectPage::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::Wheel) {
        event->ignore();
        return true;
    }
    return QWidget::eventFilter(object, event);
}

void ConnectPage::autoFill()
{
    QString displayName = ui.displayNameField->text();
    if (!displayName.isEmpty()) {
        if (ui.hostField->text().isEmpty())
            ui.hostField->setText(defaultValue("hosts", displayName).toString());
        if (ui.portField->value() == NORMAL_PORTS[0])
            ui.portField->setValue(defaultValue("ports", displayName, NORMAL_PORTS[0]).toInt());
        if (!ui.secureBox->isChecked())
            ui.secureBox->setChecked(defaultValue("secures", displayName, false).toBool());
        if (ui.serverField->toPlainText().isEmpty())
            ui.serverField->setPlainText(defaultValue("servers", displayName).toString());
        if (!ui.expandButton->isChecked() && !ui.serverField->toPlainText().isEmpty())
            ui.expandButton->setChecked(true);
        if (ui.nickNameField->toPlainText().isEmpty())
            ui.nickNameField->setPlainText(defaultValue("nickNames", displayName).toString());
        if (ui.realNameField->text().isEmpty())
            ui.realNameField->setText(defaultValue("realNames", displayName).toString());
        if (ui.userNameField->text().isEmpty())
            ui.userNameField->setText(defaultValue("userNames", displayName).toString());
        if (!ui.saslBox->isChecked())
            ui.saslBox->setChecked(defaultValue("sasls", displayName, false).toBool());
    }
}

void ConnectPage::onPortChanged(int port)
{
    if (port == SSL_PORTS[0] || port == SSL_PORTS[1] || port == SSL_PORTS[2])
        ui.secureBox->setChecked(true);
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

//    ui.displayNameField->setText(credentials.value("displayName").toString());
//    ui.hostField->setText(credentials.value("host").toString());
//    ui.portField->setValue(credentials.value("port", NORMAL_PORTS[0]).toInt());
//    ui.secureBox->setChecked(credentials.value("secure", false).toBool());
//    ui.serverField->setPlainText(credentials.value("server").toString());
//    ui.nickNameField->setPlainText(credentials.value("nickName").toString());
//    ui.realNameField->setText(credentials.value("realName").toString());
//    ui.userNameField->setText(credentials.value("userName").toString());
//    ui.saslBox->setChecked(credentials.value("sasl", false).toBool());

//    SimpleCrypt crypto(Q_UINT64_C(0x600af3d6a24df27c));
//    ui.passwordField->setText(crypto.decryptToString(credentials.value("password").toString()));

    ui.displayNameCompleter = createCompleter(credentials.value("displayNames").toStringList(), ui.displayNameField);
    ui.hostCompleter = createCompleter(credentials.value("allHosts").toStringList(), ui.hostField);
    ui.realNameCompleter = createCompleter(credentials.value("allRealNames").toStringList(), ui.realNameField);
    ui.userNameCompleter = createCompleter(credentials.value("allUserNames").toStringList(), ui.userNameField);
}

void ConnectPage::saveSettings()
{
    const QString displayName = ui.displayNameField->text();
    const QString host = ui.hostField->text();
    const int port = ui.portField->value();
    const bool secure = ui.secureBox->isChecked();
    const QString server = ui.serverField->toPlainText();
    const QString nickName = ui.nickNameField->toPlainText();
    const QString realName = ui.realNameField->text();
    const QString userName = ui.userNameField->text();
    const bool sasl = ui.saslBox->isChecked();

    QSettings settings;
    QVariantMap credentials = settings.value("credentials").toMap();
//    credentials.insert("displayName", displayName);
//    credentials.insert("host", host);
//    credentials.insert("port", port);
//    credentials.insert("secure", secure);
//    credentials.insert("server", server);
//    credentials.insert("nickName", nickName);
//    credentials.insert("realName", realName);
//    credentials.insert("userName", userName);
//    credentials.insert("sasl", sasl);

//    SimpleCrypt crypto(Q_UINT64_C(0x600af3d6a24df27c));
//    credentials.insert("password", crypto.encryptToString(ui.passwordField->text()));

    if (!displayName.isEmpty()) {
        QStringList displayNames = credentials.value("displayNames").toStringList();
        if (!displayNames.contains(displayName, Qt::CaseInsensitive))
            credentials.insert("displayNames", displayNames << displayName);
    }

    if (!host.isEmpty()) {
        QStringList allHosts = credentials.value("allHosts").toStringList();
        if (!allHosts.contains(host, Qt::CaseInsensitive))
            credentials.insert("allHosts", allHosts << host);

        QMap<QString, QVariant> hosts = credentials.value("hosts").toMap();
        hosts.insert(ConnectPage::displayName(), host);
        credentials.insert("hosts", hosts);
    }

    QMap<QString, QVariant> ports = credentials.value("ports").toMap();
    ports.insert(ConnectPage::displayName(), port);
    credentials.insert("ports", ports);

    QMap<QString, QVariant> secures = credentials.value("secures").toMap();
    secures.insert(ConnectPage::displayName(), secure);
    credentials.insert("secures", secures);

    if (!server.isEmpty()) {
        QMap<QString, QVariant> servers = credentials.value("servers").toMap();
        servers.insert(ConnectPage::displayName(), server);
        credentials.insert("servers", servers);
    }

    if (!nickName.isEmpty()) {
        QMap<QString, QVariant> nickNames = credentials.value("nickNames").toMap();
        nickNames.insert(ConnectPage::displayName(), nickName);
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

    QMap<QString, QVariant> sasls = credentials.value("sasls").toMap();
    sasls.insert(ConnectPage::displayName(), sasl);
    credentials.insert("sasls", sasls);

    settings.setValue("credentials", credentials);
}

void ConnectPage::updateUi()
{
    int lineSpacing = fontMetrics().lineSpacing();
    int margin = ui.serverField->document()->documentMargin();
    ui.serverField->setFixedHeight(qMax(2, ui.serverField->blockCount()) * lineSpacing + 2 * margin);
    ui.nickNameField->setFixedHeight(qMax(2, ui.nickNameField->blockCount()) * lineSpacing + 2 * margin);

    bool hasName = !displayName().isEmpty();
    bool hasServer = !servers().isEmpty();
    bool hasNick = !nickNames().isEmpty();
    bool hasReal = !realName().isEmpty();
    bool hasUser = !userName().isEmpty();
    bool hasPass = !password().isEmpty();

    bool validServers = true;
    foreach (const QString& server, servers()) {
        if (!IrcConnection::isValidServer(server))
            validServers = false;
    }

    ui.serverField->setProperty("error", !validServers);
    ui.serverField->setStyleSheet(QString()); // force re-style

    QPushButton* resetButton = ui.buttonBox->button(QDialogButtonBox::Reset);
    resetButton->setEnabled(hasName || hasServer || hasNick || hasReal || hasUser || hasPass);

    QPushButton* okButton = ui.buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(validServers && hasServer && hasNick);
}

void ConnectPage::reset()
{
    ui.displayNameField->clear();
    ui.hostField->clear();
    ui.portField->setValue(NORMAL_PORTS[0]);
    ui.secureBox->setChecked(false);
    ui.serverField->clear();
    ui.nickNameField->clear();
    ui.realNameField->clear();
    ui.userNameField->clear();
    ui.passwordField->clear();
    ui.saslBox->setChecked(false);
    ui.displayNameCompleter->setModel(new QStringListModel(ui.displayNameCompleter));
    ui.hostCompleter->setModel(new QStringListModel(ui.hostCompleter));
    ui.realNameCompleter->setModel(new QStringListModel(ui.realNameCompleter));
    ui.userNameCompleter->setModel(new QStringListModel(ui.userNameCompleter));
    // QSettings().remove("credentials");
    saveSettings();
}

void ConnectPage::expand(bool expand)
{
    if (expand) {
        ui.serverLabel->show();
        ui.serverField->show();
        ui.connectionLayout->addRow(ui.serverLabel, ui.serverField);
    } else {
        ui.serverLabel->hide();
        ui.serverField->hide();
        ui.connectionLayout->removeWidget(ui.serverLabel);
        ui.connectionLayout->removeWidget(ui.serverField);
    }
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

#ifdef Q_OS_MAC
    ui.serverField->setAttribute(Qt::WA_MacShowFocusRect);
    ui.nickNameField->setAttribute(Qt::WA_MacShowFocusRect);
#endif // Q_OS_MAC

    ui.scrollArea->horizontalScrollBar()->setStyle(ScrollBarStyle::narrow());
    ui.scrollArea->verticalScrollBar()->setStyle(ScrollBarStyle::narrow());

    ui.connection = connection;
    ui.displayNameCompleter = 0;
    ui.hostCompleter = 0;
    ui.nickNameCompleter = 0;
    ui.realNameCompleter = 0;
    ui.userNameCompleter = 0;

    QRegExpValidator* validator = new QRegExpValidator(this);
    validator->setRegExp(QRegExp("\\S*"));
    ui.hostField->setValidator(validator);
    ui.userNameField->setValidator(validator);

    ui.expandButton->setStyleSheet("QToolButton:!hover { border: none; background: transparent; }");
    connect(ui.expandButton, SIGNAL(toggled(bool)), this, SLOT(expand(bool)));
    expand(false);

    ui.serverField->viewport()->installEventFilter(this);
    ui.nickNameField->viewport()->installEventFilter(this);

    ui.realNameField->setPlaceholderText(ui.realNameField->placeholderText().arg(IRC_VERSION_STR));
#if QT_VERSION >= 0x050300
    qsrand(QTime::currentTime().msec());
    ui.nickNameField->setPlaceholderText(tr("Communi%1").arg(qrand() % 9999));
    if (IrcConnection::isSecureSupported())
        ui.serverField->setPlaceholderText(tr("irc.freenode.net +6697"));
    else
        ui.serverField->setPlaceholderText(tr("irc.freenode.net 6667"));
#endif // QT_VERSION

    connect(ui.buttonBox, SIGNAL(accepted()), ui.displayNameField, SLOT(setFocus()));
    connect(ui.buttonBox, SIGNAL(rejected()), ui.displayNameField, SLOT(setFocus()));

    connect(ui.buttonBox, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SIGNAL(rejected()));
    connect(ui.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));

    connect(ui.displayNameField, SIGNAL(textChanged(QString)), this, SLOT(autoFill()));
    connect(ui.portField, SIGNAL(valueChanged(int)), this, SLOT(onPortChanged(int)));

    connect(ui.displayNameField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.hostField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.serverField, SIGNAL(textChanged()), this, SLOT(updateUi()));
    connect(ui.nickNameField, SIGNAL(textChanged()), this, SLOT(updateUi()));
    connect(ui.realNameField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.userNameField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.passwordField, SIGNAL(textChanged(QString)), this, SLOT(updateUi()));
    connect(ui.portField, SIGNAL(valueChanged(int)), this, SLOT(updateUi()));
    connect(ui.secureBox, SIGNAL(toggled(bool)), this, SLOT(updateUi()));

    int labelWidth = 0;
    QList<QLabel*> labels;
    labels << ui.displayNameLabel << ui.hostLabel << ui.portLabel << ui.serverLabel;
    labels << ui.nickNameLabel << ui.realNameLabel << ui.userNameLabel << ui.passwordLabel;
    foreach (QLabel* label, labels)
        labelWidth = qMax(labelWidth, label->sizeHint().width());
    foreach (QLabel* label, labels)
        label->setMinimumWidth(labelWidth);

    ui.secureBox->setEnabled(IrcConnection::isSecureSupported());

    QShortcut* shortcut = new QShortcut(Qt::Key_Return, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Ok), SLOT(click()));

    shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Cancel), SLOT(click()));

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(saveSettings()));

    restoreSettings();
    updateUi();
}
