/*
* Copyright (C) 2008-2014 The Communi Project
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
*/

#ifndef MESSAGESTACKVIEW_H
#define MESSAGESTACKVIEW_H

#include <QHash>
#include <QStackedWidget>
#include "messagehandler.h"
#include "commandparser.h"
#include "bufferview.h"
#include "viewinfo.h"

class IrcBuffer;
class IrcConnection;
class IrcMessage;
class IrcLagTimer;
class BufferView;
class IrcBufferModel;

class MessageStackView : public QStackedWidget
{
    Q_OBJECT

public:
    MessageStackView(IrcConnection* connection, QWidget* parent = 0);

    IrcConnection* connection() const;
    CommandParser* parser() const;

    BufferView* currentView() const;
    BufferView* viewAt(int index) const;
    BufferView* bufferView(IrcBuffer* buffer) const;

public slots:
    BufferView* addView(const QString& receiver);
    void restoreView(const ViewInfo& view);
    void openView(const QString& receiver);
    void removeView(const QString& receiver);
    void closeView(int index);
    void renameView(const QString& from, const QString& to);
    void sendMessage(const QString& receiver, const QString& message);

signals:
    void viewAdded(BufferView* view);
    void viewRemoved(BufferView* view);
    void viewRenamed(BufferView* view);
    void viewActivated(BufferView* view);

private slots:
    void applySettings();
    void activateView(int index);
    void setBuffer(IrcBuffer* buffer);

private:
    BufferView* createView(ViewInfo::Type type, const QString& receiver);

    struct Private {
        IrcConnection* connection;
        CommandParser parser;
        IrcLagTimer* lagTimer;
        MessageHandler handler;
        IrcBufferModel* bufferModel;
        QHash<QString, BufferView*> views;
    } d;
};

#endif // MESSAGESTACKVIEW_H
