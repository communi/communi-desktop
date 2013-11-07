/*
 * Copyright (C) 2008-2013 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#include "connectpage.h"
#include "simplecrypt.h"
#include <QRegExpValidator>
#include <QStringList>
#include <QPushButton>
#include <QCompleter>
#include <QSettings>
#include <QShortcut>
#include <QTime>
#include <Irc>

static const int SSL_PORTS[] = { 6697, 7000, 7070 };
static const int NORMAL_PORTS[] = { 6667, 6666, 6665 };

ConnectPage::ConnectPage(QWidget* parent) : QWidget(parent)
{
    ui.setupUi(this);

    ui.displayNameCompleter = 0;
    ui.hostCompleter = 0;
    ui.nickNameCompleter = 0;
    ui.realNameCompleter = 0;
    ui.userNameCompleter = 0;

    QRegExpValidator* validator = new QRegExpValidator(this);
    validator->setRegExp(QRegExp("\\S*"));
    ui.hostField->setValidator(validator);
    ui.nickNameField->setValidator(validator);
    ui.userNameField->setValidator(validator);

    qsrand(QTime::currentTime().msec());
    ui.nickNameField->setPlaceholderText(ui.nickNameField->placeholderText().arg(qrand() % 9999));
    ui.realNameField->setPlaceholderText(ui.realNameField->placeholderText().arg(IRC_VERSION_STR));

    connect(ui.buttonBox, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SIGNAL(rejected()));
    connect(ui.buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));

    connect(ui.hostField, SIGNAL(textChanged(QString)), this, SLOT(onHostFieldChanged()));
    connect(ui.portField, SIGNAL(valueChanged(int)), this, SLOT(onPortFieldChanged(int)));
    connect(ui.secureField, SIGNAL(toggled(bool)), this, SLOT(onSecureFieldToggled(bool)));

    QShortcut* shortcut = new QShortcut(Qt::Key_Return, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Ok), SLOT(click()));

    shortcut = new QShortcut(Qt::Key_Escape, this);
    connect(shortcut, SIGNAL(activated()), ui.buttonBox->button(QDialogButtonBox::Cancel), SLOT(click()));

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(saveSettings()));
    restoreSettings();
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

QString ConnectPage::host() const
{
    return fieldValue(ui.hostField->text(), ui.hostField->placeholderText());
}

int ConnectPage::port() const
{
    return ui.portField->value();
}

bool ConnectPage::isSecure() const
{
    return ui.secureField->isChecked();
}

QString ConnectPage::nickName() const
{
    return fieldValue(ui.nickNameField->text(), ui.nickNameField->placeholderText());
}

QString ConnectPage::realName() const
{
    return fieldValue(ui.realNameField->text(), ui.realNameField->placeholderText());
}

QString ConnectPage::userName() const
{
    return fieldValue(ui.userNameField->text(), ui.userNameField->placeholderText());
}

QString ConnectPage::password() const
{
    return ui.passwordField->text();
}

QDialogButtonBox* ConnectPage::buttonBox() const
{
    return ui.buttonBox;
}

static QString capitalize(const QString& str)
{
    return str.left(1).toUpper() + str.mid(1);
}

void ConnectPage::onHostFieldChanged()
{
    // pick the longest part of the url, skipping the most common domain prefixes and suffixes
    QString name;
    QStringList parts = host().split(QLatin1Char('.'), QString::SkipEmptyParts);
    while (!parts.isEmpty()) {
        QString part = parts.takeFirst();
        const QString lower = part.toLower();
        if (!QString("irc").startsWith(lower) && !QString("org").startsWith(lower)
                && !QString("net").startsWith(lower) && !QString("com").startsWith(lower)) {
            if (part.length() > name.length())
                name = part;
        }
    }
    if (!name.isEmpty())
        ui.displayNameField->setPlaceholderText(capitalize(name));
}

void ConnectPage::onPortFieldChanged(int port)
{
    if (port == SSL_PORTS[0] || port == SSL_PORTS[1] || port == SSL_PORTS[2])
        ui.secureField->setChecked(true);
}

void ConnectPage::onSecureFieldToggled(bool secure)
{
    if (secure) {
        const int port = ui.portField->value();
        if (port == NORMAL_PORTS[0] || port == NORMAL_PORTS[1] || port == NORMAL_PORTS[2])
            ui.portField->setValue(SSL_PORTS[0]);
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
    settings.beginGroup("Credentials");

    ui.displayNameField->setText(settings.value("displayName").toString());
    ui.hostField->setText(settings.value("host").toString());
    ui.portField->setValue(settings.value("port", NORMAL_PORTS[0]).toInt());
    ui.secureField->setChecked(settings.value("secure", false).toBool());
    ui.nickNameField->setText(settings.value("nickName").toString());
    ui.realNameField->setText(settings.value("realName").toString());
    ui.userNameField->setText(settings.value("userName").toString());

    SimpleCrypt crypto(Q_UINT64_C(0x600af3d6a24df27c));
    ui.passwordField->setText(crypto.decryptToString(settings.value("password").toString()));

    ui.displayNameCompleter = createCompleter(settings.value("displayNames").toStringList(), ui.displayNameField);
    ui.hostCompleter = createCompleter(settings.value("hosts").toStringList(), ui.hostField);
    ui.nickNameCompleter = createCompleter(settings.value("nickNames").toStringList(), ui.nickNameField);
    ui.realNameCompleter = createCompleter(settings.value("realNames").toStringList(), ui.realNameField);
    ui.userNameCompleter = createCompleter(settings.value("userNames").toStringList(), ui.userNameField);
}

void ConnectPage::saveSettings()
{
    QSettings settings;
    settings.beginGroup("Credentials");

    const QString displayName = ui.displayNameField->text();
    const QString host = ui.hostField->text();
    const int port = ui.portField->value();
    const bool secure = ui.secureField->isChecked();
    const QString nickName = ui.nickNameField->text();
    const QString realName = ui.realNameField->text();
    const QString userName = ui.userNameField->text();

    settings.setValue("displayName", displayName);
    settings.setValue("host", host);
    settings.setValue("port", port);
    settings.setValue("secure", secure);
    settings.setValue("nickName", nickName);
    settings.setValue("realName", realName);
    settings.setValue("userName", userName);

    SimpleCrypt crypto(Q_UINT64_C(0x600af3d6a24df27c));
    settings.setValue("password", crypto.encryptToString(ui.passwordField->text()));

    if (!displayName.isEmpty()) {
        QStringList displayNames = settings.value("displayNames").toStringList();
        if (!displayNames.contains(displayName, Qt::CaseInsensitive))
            settings.setValue("displayNames", displayNames << displayName);
    }

    if (!host.isEmpty()) {
        QStringList hosts = settings.value("hosts").toStringList();
        if (!hosts.contains(host, Qt::CaseInsensitive))
            settings.setValue("hosts", hosts << host);
    }

    if (port != NORMAL_PORTS[0]) {
        QMap<QString, QVariant> ports = settings.value("ports").toMap();
        ports.insert(ConnectPage::host(), port);
        settings.setValue("ports", ports);
    }

    if (secure) {
        QMap<QString, QVariant> secures = settings.value("secures").toMap();
        secures.insert(ConnectPage::host(), secure);
        settings.setValue("secures", secures);
    }

    if (!nickName.isEmpty()) {
        QStringList nickNames = settings.value("nickNames").toStringList();
        if (!nickNames.contains(nickName, Qt::CaseInsensitive))
            settings.setValue("nickNames", nickNames << nickName);
    }

    if (!realName.isEmpty()) {
        QStringList realNames = settings.value("realNames").toStringList();
        if (!realNames.contains(realName, Qt::CaseInsensitive))
            settings.setValue("realNames", realNames << realName);
    }

    if (!userName.isEmpty()) {
        QStringList userNames = settings.value("userNames").toStringList();
        if (!userNames.contains(userName, Qt::CaseInsensitive))
            settings.setValue("userNames", userNames << userName);
    }
}

void ConnectPage::reset()
{
    ui.displayNameField->clear();
    ui.hostField->clear();
    ui.portField->setValue(NORMAL_PORTS[0]);
    ui.secureField->setChecked(false);
    ui.nickNameField->clear();
    ui.realNameField->clear();
    ui.userNameField->clear();
    ui.passwordField->clear();
    saveSettings();
}
