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

#ifndef NICKHIGHLIGHTER_H
#define NICKHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QHash>
#include <QList>

class NickHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
    NickHighlighter(QTextDocument* parent);

protected:
	void highlightBlock(const QString& text);

private:
    static QList<QColor> colors;
};

#endif // NICKHIGHLIGHTER_H
