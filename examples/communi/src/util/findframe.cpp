/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#include "findframe.h"
#include <QTextBrowser>
#include <QShortcut>

FindFrame::FindFrame(QWidget* parent) : QFrame(parent)
{
    ui.setupUi(this);

    Q_ASSERT(parent);
    QShortcut* shortcut = new QShortcut(QKeySequence::Find, parent);
    connect(shortcut, SIGNAL(activated()), this, SLOT(find()));

    shortcut = new QShortcut(QKeySequence::FindNext, parent);
    connect(shortcut, SIGNAL(activated()), this, SLOT(findNext()));
    connect(ui.toolNext, SIGNAL(clicked()), this, SLOT(findNext()));
    connect(ui.editFind, SIGNAL(returnPressed()), this, SLOT(findNext()));
    connect(ui.editFind, SIGNAL(textEdited(QString)), this, SLOT(find(QString)));

    shortcut = new QShortcut(QKeySequence::FindPrevious, parent);
    connect(shortcut, SIGNAL(activated()), this, SLOT(findPrevious()));
    connect(ui.toolPrevious, SIGNAL(clicked()), this, SLOT(findPrevious()));

    connect(ui.toolClose, SIGNAL(clicked()), this, SLOT(hide()));

    setVisible(false);

#ifdef Q_WS_MAEMO_5
    delete ui.checkCase;
    ui.checkCase = 0;
    delete ui.checkWholeWords;
    ui.checkWholeWords = 0;
    delete ui.labelWrapped;
    ui.labelWrapped = 0;
#else
    ui.labelWrapped->setVisible(false);
#endif // Q_WS_MAEMO_5
}

QTextEdit* FindFrame::textEdit() const
{
    return ui.textEdit;
}

void FindFrame::setTextEdit(QTextEdit* textEdit)
{
    ui.textEdit = textEdit;
}

void FindFrame::findNext()
{
    find(ui.editFind->text(), true, false);
}

void FindFrame::findPrevious()
{
    find(ui.editFind->text(), false, true);
}

void FindFrame::find()
{
    show();
    ui.editFind->setFocus(Qt::ShortcutFocusReason);
    ui.editFind->selectAll();
}

void FindFrame::find(const QString& text, bool forward, bool backward)
{
    if (!ui.textEdit)
        return;

    QTextDocument* doc = ui.textEdit->document();
    QTextCursor cursor = ui.textEdit->textCursor();
    QString oldText = ui.editFind->text();

    QTextDocument::FindFlags options;
    QPalette pal = ui.editFind->palette();
    pal.setColor(QPalette::Active, QPalette::Base, Qt::white);

    if (cursor.hasSelection())
        cursor.setPosition(forward ? cursor.position() : cursor.anchor(), QTextCursor::MoveAnchor);

    QTextCursor newCursor = cursor;

    if (!text.isEmpty()) {
        if (backward)
            options |= QTextDocument::FindBackward;

        if (ui.checkCase && ui.checkCase->isChecked())
            options |= QTextDocument::FindCaseSensitively;

        if (ui.checkWholeWords && ui.checkWholeWords->isChecked())
            options |= QTextDocument::FindWholeWords;

        newCursor = doc->find(text, cursor, options);
        if (ui.labelWrapped)
            ui.labelWrapped->hide();

        if (newCursor.isNull()) {
            QTextCursor ac(doc);
            ac.movePosition(options & QTextDocument::FindBackward
                ? QTextCursor::End : QTextCursor::Start);
            newCursor = doc->find(text, ac, options);
            if (newCursor.isNull()) {
                pal.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
                newCursor = cursor;
            } else if (ui.labelWrapped)
                ui.labelWrapped->show();
        }
    }

    if (!isVisible())
        show();
    ui.textEdit->setTextCursor(newCursor);
    ui.editFind->setPalette(pal);
}
