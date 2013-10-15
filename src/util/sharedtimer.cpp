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

#include "sharedtimer.h"
#include <QTimerEvent>

SharedTimer::SharedTimer(QObject* parent) : QObject(parent)
{
    d.interval = 500;
}

SharedTimer* SharedTimer::instance()
{
    static SharedTimer timer;
    return &timer;
}

int SharedTimer::interval() const
{
    return d.interval;
}

void SharedTimer::setInterval(int interval)
{
    if (d.timer.isActive())
        qWarning("SharedTimer::setInterval(): timer active");
    d.interval = interval;
}

void SharedTimer::registerReceiver(QObject* receiver, const QByteArray& member)
{
    if (!receiver || member.isEmpty())
        return;

    QList<QByteArray> members = d.members.values(receiver);
    if (!members.contains(member)) {
        if (d.members.isEmpty())
            d.timer.start(d.interval, this);

        if (!d.members.contains(receiver))
            connect(receiver, SIGNAL(destroyed(QObject*)), this, SLOT(destroyed(QObject*)));

        d.members.insertMulti(receiver, member);
    }
}

void SharedTimer::unregisterReceiver(QObject* receiver, const QByteArray& member)
{
    if (!receiver)
        return;

    if (member.isNull())
        d.members.remove(receiver);
    else
        d.members.remove(receiver, member);

    if (!d.members.contains(receiver))
        disconnect(receiver, SIGNAL(destroyed(QObject*)), this, SLOT(destroyed(QObject*)));

    if (d.members.isEmpty())
        d.timer.stop();
}

void SharedTimer::pause()
{
    if (d.timer.isActive())
        d.timer.stop();
}

void SharedTimer::resume()
{
    if (!d.members.isEmpty() && !d.timer.isActive())
        d.timer.start(d.interval, this);
}

void SharedTimer::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == d.timer.timerId()) {
        QHashIterator<QObject*, QByteArray> it(d.members);
        while (it.hasNext()) {
            it.next();
            QMetaObject::invokeMethod(it.key(), it.value());
        }
    }
}

void SharedTimer::destroyed(QObject* object)
{
    unregisterReceiver(object);
}
