/*
* Copyright (C) 2008-2013 The Communi Project
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

#include "completer.h"
#include "lineeditor.h"
#include "ircusermodel.h"

Completer::Completer(QObject* parent) : QCompleter(parent)
{
    d.editor = 0;
    d.userModel = 0;
    d.channelModel = 0;
    d.commandModel = 0;
    d.channelPrefixes = QLatin1String("#");
    d.commandPrefixes = QLatin1String("/");
    setCaseSensitivity(Qt::CaseInsensitive);
    setCompletionMode(InlineCompletion);
    connect(this, SIGNAL(highlighted(QString)), this, SLOT(insertCompletion(QString)));
}

LineEditor* Completer::editor() const
{
    return d.editor;
}

void Completer::setEditor(LineEditor* editor)
{
    if (d.editor != editor) {
        if (d.editor)
            disconnect(d.editor, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited()));

        setWidget(editor);

        if (editor)
            connect(editor, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited()));

        d.editor = editor;
    }
}

QAbstractItemModel* Completer::userModel() const
{
    return d.userModel;
}

void Completer::setUserModel(QAbstractItemModel* model)
{
    d.userModel = model;
}

QAbstractItemModel* Completer::channelModel() const
{
    return d.channelModel;
}

void Completer::setChannelModel(QAbstractItemModel* model)
{
    d.channelModel = model;
}

QAbstractItemModel* Completer::commandModel() const
{
    return d.commandModel;
}

void Completer::setCommandModel(QAbstractItemModel* model)
{
    d.commandModel = model;
}

QString Completer::channelPrefixes() const
{
    return d.channelPrefixes;
}

void Completer::setChannelPrefixes(const QString& prefixes)
{
    d.channelPrefixes = prefixes;
}

QString Completer::commandPrefixes() const
{
    return d.commandPrefixes;
}

void Completer::setCommandPrefixes(const QString& prefixes)
{
    d.commandPrefixes = prefixes;
}

void Completer::onTabPressed()
{
    if (!d.editor)
        return;

    // store selection
    int pos = d.editor->cursorPosition();
    int start = d.editor->selectionStart();
    QString selected = d.editor->selectedText();

    // select current word
    d.editor->cursorWordForward(false);
    d.editor->cursorWordBackward(true);
    QString word = d.editor->selectedText();

    // choose model
    if (!word.isEmpty() && d.commandPrefixes.contains(word.at(0))) {
        if (model() != d.commandModel) {
            setModel(d.commandModel);
            setCompletionRole(Qt::DisplayRole);
        }
    } else if (!word.isEmpty() && d.channelPrefixes.contains(word.at(0))) {
        if (model() != d.channelModel) {
            setModel(d.channelModel);
            setCompletionRole(Qt::DisplayRole);
        }
    } else {
        if (model() != d.userModel) {
            setModel(d.userModel);
            setCompletionRole(Irc::NameRole);
        }
    }

    bool repeat = true;
    QString prefix = completionPrefix();
    if (prefix.isEmpty() || !word.startsWith(prefix, Qt::CaseInsensitive)) {
        repeat = false;
        setCompletionPrefix(word);
    }

    // restore selection
    d.editor->setCursorPosition(pos);
    if (start != -1)
        d.editor->setSelection(start, selected.length());

    // complete
    if (!word.isEmpty()) {
        complete();

        int count = completionCount();
        if (count > 1) {
            int next = currentRow() + 1;
            setCurrentRow(next % count);
        } else if (count == 1 && repeat && d.commandPrefixes.contains(word.at(0))) {
            emit commandCompletion(word.mid(1));
        }
    }
}

void Completer::onTextEdited()
{
    setCompletionPrefix(QString());
}

void Completer::insertCompletion(const QString& completion)
{
    if (!d.editor || completion.isEmpty())
        return;

    int pos = d.editor->cursorPosition();
    QString text = d.editor->text();
    if (pos > 0 && pos < text.length() && !text.at(pos - 1).isSpace())
        d.editor->cursorWordForward(false);
    d.editor->cursorWordBackward(true);
    pos = d.editor->cursorPosition();

    QString tmp = completion;
    if (pos == 0 && !d.channelPrefixes.contains(completion.at(0))
                 && !d.commandPrefixes.contains(completion.at(0)))
        tmp.append(":");
    d.editor->insert(tmp);

    text = d.editor->text();
    int cursor = d.editor->cursorPosition();
    if (!text.at(cursor - 1).isSpace())
        d.editor->insert(" ");
}
