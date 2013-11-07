/*
 * Copyright (C) 2008-2013 The Communi Project
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
    QString host() const;
    int port() const;
    bool isSecure() const;
    QString nickName() const;
    QString realName() const;
    QString userName() const;
    QString password() const;

    QDialogButtonBox* buttonBox() const;

signals:
    void accepted();
    void rejected();

private slots:
    void onHostFieldChanged();
    void onPortFieldChanged(int port);
    void onSecureFieldToggled(bool secure);
    void restoreSettings();
    void saveSettings();
    void reset();

private:
    struct Private : public Ui::ConnectPage {
        QCompleter* displayNameCompleter;
        QCompleter* hostCompleter;
        QCompleter* nickNameCompleter;
        QCompleter* realNameCompleter;
        QCompleter* userNameCompleter;
    } ui;
};

#endif // CONNECTPAGE_H
