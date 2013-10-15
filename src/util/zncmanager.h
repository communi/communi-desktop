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

#ifndef ZNCMANAGER_H
#define ZNCMANAGER_H

#include <QObject>
#include <QElapsedTimer>
#include <IrcBufferModel>
#include <IrcMessageFilter>

class IrcNoticeMessage;
class IrcPrivateMessage;

class ZncManager : public QObject, public IrcMessageFilter
{
    Q_OBJECT
    Q_INTERFACES(IrcMessageFilter)
    Q_PROPERTY(bool playbackActive READ isPlaybackActive NOTIFY playbackActiveChanged)
    Q_PROPERTY(QString playbackTarget READ playbackTarget NOTIFY playbackTargetChanged)
    Q_PROPERTY(IrcBufferModel* model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QString timeStampFormat READ timeStampFormat WRITE setTimeStampFormat NOTIFY timeStampFormatChanged)

public:
    explicit ZncManager(QObject* parent = 0);
    virtual ~ZncManager();

    bool isPlaybackActive() const;
    QString playbackTarget() const;

    IrcBufferModel* model() const;
    void setModel(IrcBufferModel* model);

    QString timeStampFormat() const;
    void setTimeStampFormat(const QString& format);

    bool messageFilter(IrcMessage* message);

signals:
    void playbackActiveChanged(bool active);
    void modelChanged(IrcBufferModel* model);
    void playbackTargetChanged(const QString& target);
    void timeStampFormatChanged(const QString& format);

protected:
    bool processMessage(IrcPrivateMessage* message);
    bool processNotice(IrcNoticeMessage* message);

private slots:
    void onConnected();
    void requestCapabilities();

private:
    mutable struct Private {
        bool playback;
        long timestamp;
        QString target;
        IrcBuffer* buffer;
        IrcBufferModel* model;
        QString timeStampFormat;
        QElapsedTimer timestamper;
    } d;
};

#endif // ZNCMANAGER_H
