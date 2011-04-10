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
        colors << QColor(QLatin1String("#BF0000"))
            << QColor(QLatin1String("#FF0000"))
            << QColor(QLatin1String("#BF7900"))
            << QColor(QLatin1String("#00BC0F"))
            << QColor(QLatin1String("#00A7BA"))
            << QColor(QLatin1String("#0F00B7"))
            << QColor(QLatin1String("#8100B5"));
    }
}

void NickHighlighter::highlightBlock(const QString& text)
{
    QRegExp lineRegExp("(\\[[^\\]]+\\] )?<[^>]+>");
    if (text.indexOf(lineRegExp) == 0)
    {
        QRegExp nickRegExp("<[^>]+>");
        int index = text.indexOf(nickRegExp);
        if (index != -1)
        {
            int length = nickRegExp.matchedLength();
            if (length != -1)
            {
                QColor color;
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
    }
}
