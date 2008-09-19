/*
* Copyright (C) 2008 J-P Nurmi jpnurmi@gmail.com
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* $Id$
*/

#include "nickhighlighter.h"

struct TextBlockUserData : public QTextBlockUserData
{
    QString nick;
    QColor color;
};

QList<QColor> NickHighlighter::colors;

NickHighlighter::NickHighlighter(QTextDocument* parent)
	: QSyntaxHighlighter(parent)
{
    //http://trolltech.com/developer/task-tracker/index_html?method=entry&id=196704
    //for (int h = 0; h < 360; h += 30)
        //colors += QColor::fromHsv(h, saturation, 100);

    if (colors.isEmpty())
    {
        colors << QColor("#BF0000")
            << QColor("#FF0000")
            << QColor("#BF7900")
            << QColor("#00BC0F")
            << QColor("#00A7BA")
            << QColor("#0F00B7")
            << QColor("#8100B5");
    }
}

void NickHighlighter::highlightBlock(const QString& text)
{
    // message highlight
    if (currentBlockState() != -1)
        setFormat(0, text.length(), Qt::red);

    // nick colors
    static const QRegExp regExp("<[^>]+>");
    int index = text.indexOf(regExp);
    if (index != -1)
    {
        QColor color;
        int length = regExp.matchedLength();

        TextBlockUserData* data = dynamic_cast<TextBlockUserData*>(currentBlockUserData());
        if (data)
        {
            color = data->color;
        }
        else
        {
            data = new TextBlockUserData;
            data->nick = text.mid(index + 1, length - 2).toLower();
            data->color = colors.at(qHash(data->nick) % colors.count());
            setCurrentBlockUserData(data);
        }

        if (data->color.isValid())
            setFormat(index + 1, length - 2, data->color);
    }
}
