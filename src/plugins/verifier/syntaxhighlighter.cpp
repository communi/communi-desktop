/*
* Copyright (C) 2008-2014 Communi authors
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

#include "syntaxhighlighter.h"
#include <QPalette>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* document) : QSyntaxHighlighter(document)
{
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    Q_UNUSED(text);
    if (currentBlockState() > 0) {
        // gray-out everything except links
        QTextBlock block = currentBlock();
        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            QTextFragment fragment = it.fragment();
            if (fragment.isValid() && !fragment.charFormat().isAnchor())
                setFormat(fragment.position() - block.position(), fragment.length(), QPalette().color(QPalette::Disabled, QPalette::Text));
        }
    }
}
