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

#ifndef CONNECTPAGE_H
#define CONNECTPAGE_H

#include "ui_connectpage.h"

class IrcConnection;

class ConnectPage : public QWidget
{
    Q_OBJECT

public:
    ConnectPage(QWidget* parent = 0);
    ConnectPage(IrcConnection* connection, QWidget* parent = 0);
    ~ConnectPage();

    QString displayName() const;
    void setDisplayName(const QString& name);

    QStringList servers() const;
    void setServers(const QStringList& servers);

    QString saslMechanism() const;
    void setSaslMechanism(const QString& mechanism);

    QStringList nickNames() const;
    void setNickNames(const QStringList& names);

    QString userName() const;
    void setUserName(const QString& name);

    QString realName() const;
    void setRealName(const QString& name);

    QString password() const;
    void setPassword(const QString& password);

    IrcConnection* connection() const;
    QDialogButtonBox* buttonBox() const;

signals:
    void accepted();
    void rejected();

protected:
    bool eventFilter(QObject* object, QEvent* event);

private slots:
    void autoFill();
    void onPortChanged(int port);
    void restoreSettings();
    void saveSettings();
    void updateUi();
    void reset();
    void expand(bool expand);

private:
    QVariant defaultValue(const QString& key, const QString& field, const QVariant& defaultValue = QVariant()) const;

    void init(IrcConnection* connection = 0);

    struct Private : public Ui::ConnectPage {
        IrcConnection* connection;
        QCompleter* displayNameCompleter;
        QCompleter* hostCompleter;
        QCompleter* nickNameCompleter;
        QCompleter* realNameCompleter;
        QCompleter* userNameCompleter;
    } ui;
};

#endif // CONNECTPAGE_H
