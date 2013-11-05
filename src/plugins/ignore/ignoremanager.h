/*
* Copyright (C) 2008-2013 The Communi Project
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef IGNOREMANAGER_H
#define IGNOREMANAGER_H

#include <QObject>
#include <QStringList>
#include <IrcMessageFilter>

class IrcMessage;
class IrcConnection;

class IgnoreManager : public QObject, public IrcMessageFilter
{
    Q_OBJECT
    Q_INTERFACES(IrcMessageFilter)
    Q_PROPERTY(QStringList ignores READ ignores WRITE setIgnores)

public:
    static IgnoreManager* instance();
    virtual ~IgnoreManager();

    bool messageFilter(IrcMessage* message);

    QStringList ignores() const;

public slots:
    QString addIgnore(const QString& ignore);
    QString removeIgnore(const QString& ignore);
    void setIgnores(const QStringList& ignores);

    void addConnection(IrcConnection* connection);
    void removeConnection(IrcConnection* connection);

private:
    explicit IgnoreManager(QObject* parent = 0);

    struct Private {
        QStringList ignores;
    } d;
};

#endif // IGNOREMANAGER_H
