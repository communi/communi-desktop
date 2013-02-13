/*
* Copyright (C) 2008-2013 Communi authors
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
#include "historylineedit.h"

Completer::Completer(QObject* parent) : QCompleter(parent)
{
    d.lineEdit = 0;
    d.userModel = 0;
    d.commandModel = 0;
    setCaseSensitivity(Qt::CaseInsensitive);
    setCompletionMode(InlineCompletion);
    connect(this, SIGNAL(highlighted(QString)), this, SLOT(insertCompletion(QString)));
}

HistoryLineEdit* Completer::lineEdit() const
{
    return d.lineEdit;
}

void Completer::setLineEdit(HistoryLineEdit* lineEdit)
{
    if (d.lineEdit != lineEdit) {
        if (d.lineEdit)
            disconnect(d.lineEdit, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited()));

        if (lineEdit)
            connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited()));

        d.lineEdit = lineEdit;
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

QAbstractItemModel* Completer::commandModel() const
{
    return d.commandModel;
}

void Completer::setCommandModel(QAbstractItemModel* model)
{
    d.commandModel = model;
}

void Completer::onTabPressed()
{
    if (!d.lineEdit)
        return;

    // store selection
    int pos = d.lineEdit->cursorPosition();
    int start = d.lineEdit->selectionStart();
    QString selected = d.lineEdit->selectedText();

    // select current word
    d.lineEdit->cursorWordForward(false);
    d.lineEdit->cursorWordBackward(true);
    QString word = d.lineEdit->selectedText();

    // choose model
    if (word.startsWith('/')) {
        if (model() != d.commandModel)
            setModel(d.commandModel);
    } else {
        if (model() != d.userModel)
            setModel(d.userModel);
    }

    bool repeat = true;
    QString prefix = completionPrefix();
    if (prefix.isEmpty() || !word.startsWith(prefix, Qt::CaseInsensitive)) {
        repeat = false;
        setCompletionPrefix(word);
    }

    // restore selection
    d.lineEdit->setCursorPosition(pos);
    if (start != -1)
        d.lineEdit->setSelection(start, selected.length());

    // complete
    if (!word.isEmpty()) {
        complete();

        int count = completionCount();
        if (count > 1) {
            int next = currentRow() + 1;
            setCurrentRow(next % count);
        } else if (count == 1 && repeat && word.startsWith('/')) {
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
    if (!d.lineEdit)
        return;

    int pos = d.lineEdit->cursorPosition();
    QString text = d.lineEdit->text();
    if (pos > 0 && pos < text.length() && !text.at(pos - 1).isSpace())
        d.lineEdit->cursorWordForward(false);
    d.lineEdit->cursorWordBackward(true);
    pos = d.lineEdit->cursorPosition();

    QString tmp = completion;
    if (pos == 0 && !completion.startsWith("/"))
        tmp.append(":");
    d.lineEdit->insert(tmp);

    text = d.lineEdit->text();
    int cursor = d.lineEdit->cursorPosition();
    if (!text.at(cursor - 1).isSpace())
        d.lineEdit->insert(" ");
}
