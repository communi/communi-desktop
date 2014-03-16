/*
 * Copyright (C) 2008-2014 The Communi Project
 *
 * This example is free, and not covered by the LGPL license. There is no
 * restriction applied to their modification, redistribution, using and so on.
 * You can study them, modify them, use them in your own program - either
 * completely or partially.
 */

#ifndef CONNECTPAGE_H
#define CONNECTPAGE_H

#include "ui_connectpage.h"

class ConnectPage : public QWidget
{
    Q_OBJECT

public:
    ConnectPage(QWidget* parent = 0);
    ~ConnectPage();

    QString displayName() const;
    void setDisplayName(const QString& name);

    QString host() const;
    void setHost(const QString& host);

    int port() const;
    void setPort(int port);

    bool isSecure() const;
    void setSecure(bool secure);

    QString saslMechanism() const;
    void setSaslMechanism(const QString& mechanism);

    QString nickName() const;
    void setNickName(const QString& name);

    QString userName() const;
    void setUserName(const QString& name);

    QString realName() const;
    void setRealName(const QString& name);

    QString password() const;
    void setPassword(const QString& password);

    QDialogButtonBox* buttonBox() const;

signals:
    void accepted();
    void rejected();

private slots:
    void onDisplayNameFieldChanged();
    void onHostFieldChanged();
    void onPortFieldChanged(int port);
    void onSecureBoxToggled(bool secure);
    void restoreSettings();
    void saveSettings();
    void updateUi();
    void reset();

private:
    QVariant defaultValue(const QString& key, const QString& field, const QVariant& defaultValue = QVariant()) const;

    struct Private : public Ui::ConnectPage {
        QCompleter* displayNameCompleter;
        QCompleter* hostCompleter;
        QCompleter* nickNameCompleter;
        QCompleter* realNameCompleter;
        QCompleter* userNameCompleter;
    } ui;
};

#endif // CONNECTPAGE_H
