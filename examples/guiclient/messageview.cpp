/*
* Copyright (C) 2008 J-P Nurmi jpnurmi@gmail.com
* Copyright (C) 2008 Adam Higerd ahigerd@libqxt.org
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

#include "messageview.h"
#include "nickhighlighter.h"
#include "irc.h"

#include <QTime>
#include <QRegExp>
#include <QtDebug>

static QRegExp URL_PATTERN("((www\\.(?!\\.)|(fish|irc|(f|sf|ht)tp(|s))://)(\\.?[\\d\\w/,\\':~\\?=;#@\\-\\+\\%\\*\\{\\}\\!\\(\\)]|&)+)|([-.\\d\\w]+@[-.\\d\\w]{2,}\\.[\\w]{2,})", Qt::CaseInsensitive);

static QString messageColor(int color)
{
    switch (color)
    {
		case 0:  return QLatin1String("white");
		case 1:  return QLatin1String("black");
		case 2:  return QLatin1String("navy");
		case 3:  return QLatin1String("green");
		case 4:  return QLatin1String("red");
		case 5:  return QLatin1String("maroon");
		case 6:  return QLatin1String("purple");
		case 7:  return QLatin1String("orange");
		case 8:  return QLatin1String("yellow");
		case 9:  return QLatin1String("lime");
		case 10: return QLatin1String("darkcyan");
		case 11: return QLatin1String("cyan");
		case 12: return QLatin1String("blue");
		case 13: return QLatin1String("magenta");
		case 14: return QLatin1String("gray");
		case 15: return QLatin1String("lightgray");
		default: return QLatin1String("black");
    }
}

static QString processMessage(const QString& message)
{
    QString processed = message;

    enum 
    {
        None            = 0x0,
        Bold            = 0x1,
        Color           = 0x2,
        Italic          = 0x4,
        StrikeThrough   = 0x8,
        Underline       = 0x10,
        Inverse         = 0x20
    };
    int state = None;

    int pos = 0;
    while (pos < processed.size())
    {
        if (state & Color)
        {
            QString tmp = processed.mid(pos, 2);
            processed.remove(pos, 2);
            processed = processed.arg(messageColor(tmp.toInt()));
            state &= ~Color;
            pos += 2;
            continue;
        }

        QString replacement;
        switch (processed.at(pos).unicode())
        {
        case '\x02': // bold
            if (state & Bold)
                replacement = QLatin1String("</span>");
            else
                replacement = QLatin1String("<span style='font-weight: bold'>");
            state ^= Bold;
            break;

        case '\x03': // color
            if (state & Color)
                replacement = QLatin1String("</span>");
            else
                replacement = QLatin1String("<span style='color: %1'>");
            state ^= Color;
            break;

        case '\x09': // italic
            if (state & Italic)
                replacement = QLatin1String("</span>");
            else
                replacement = QLatin1String("<span style='text-decoration: underline'>");
            state ^= Italic;
            break;

        case '\x13': // strike-through
            if (state & StrikeThrough)
                replacement = QLatin1String("</span>");
            else
                replacement = QLatin1String("<span style='text-decoration: line-through'>");
            state ^= StrikeThrough;
            break;

        case '\x15': // underline
        case '\x1f': // underline
            if (state & Underline)
                replacement = QLatin1String("</span>");
            else
                replacement = QLatin1String("<span style='text-decoration: underline'>");
            state ^= Underline;
            break;

        case '\x16': // inverse
            state ^= Inverse;
            break;

        case '\x0f': // none
            replacement = QLatin1String("</span>");
            state = None;
            break;

        default:
            break;
        }

        if (!replacement.isNull())
        {
            processed.replace(pos, 1, replacement);
            pos += replacement.length();
        }
        else
        {
            ++pos;
        }
    }

    pos = 0;
    while ((pos = URL_PATTERN.indexIn(processed, pos)) >= 0)
    {
        int len = URL_PATTERN.matchedLength();
        QString href = processed.mid(pos, len);

        // Don't consider trailing comma or semi-colon as part of the link.
        QString append;
        if (href.endsWith(",") || href.endsWith(";"))
        {
            append.append(href.right(1));
            href.chop(1);
        }
        
        // Don't consider trailing closing parenthesis part of the link when
        // there's an opening parenthesis preceding in the beginning of the
        // URL or there is no opening parenthesis in the URL at all.
        if (href.endsWith(")") && (processed.at(pos-1) == '(' || !href.contains("(")))
        {
            append.prepend(href.right(1));
            href.chop(1);
        }
        
        // Qt doesn't support (?<=pattern) so we do it here
        if (pos > 0 && processed.at(pos-1).isLetterOrNumber())
        {
            pos++;
            continue;
        }
        
        QString protocol;
        if (URL_PATTERN.cap(1).startsWith("www.", Qt::CaseInsensitive))
            protocol = "http://";
        else if (URL_PATTERN.cap(1).isEmpty())
            protocol = "mailto:";
        
        // Use \x0b as a placeholder for & so we can readd them after changing all & in the normal text to &amp;
        // HACK Replace % with \x03 in the url to keep Qt from doing stupid things
        QString link = QString("<a href='%1%2'>%3</a>").arg(protocol, QString(href).replace('&', "\x0b").replace('%', "\x03"), href) + append;
        processed.replace(pos, len, link);
        pos += link.length();
    }
    
    return processed;
}

MessageView::MessageView(const QString& receiver, QWidget* parent)
    : QTextBrowser(parent), mReceiver(receiver)
{
    setFrameShape(QFrame::NoFrame);
    setOpenExternalLinks(true);
    new NickHighlighter(document());
}

const QString& MessageView::receiver() const
{
    return mReceiver;
}

bool MessageView::matches(const QString& receiver) const
{
    return !mReceiver.compare(receiver, Qt::CaseInsensitive);
}

void MessageView::setReceiver(const QString& receiver)
{
    mReceiver = receiver;
}

const QStringList& MessageView::nicks() const
{
    return mNicks;
}

bool MessageView::contains(const QString& nick) const
{
    return mNicks.contains(nick, Qt::CaseInsensitive);
}

void MessageView::addNick(const QString& nick)
{
    if (!mNicks.contains(nick, Qt::CaseInsensitive))
        mNicks.append(nick);
}

void MessageView::removeNick(const QString& nick)
{
    if (!mNicks.contains(nick, Qt::CaseInsensitive))
        mNicks.append(nick);
}

void MessageView::receiveNicks(const QStringList& nicks)
{
    QString msg = QString("[ %2 ]").arg(nicks.join(" ] [ "));
    logMessage(QString(), "%1! %2", msg);
}

void MessageView::receiveMessage(const QString& sender, const QString& message)
{
    logMessage(sender, "&lt;%1&gt; %2", message);
}

void MessageView::receiveNotice(const QString& sender, const QString& message)
{
    logMessage(sender, "[%1] %2", message);
}

void MessageView::receiveAction(const QString& sender, const QString& message)
{
    logMessage(sender, "* %1 %2", message);
}

void MessageView::logMessage(const QString& sender, const QString& format, const QString& message)
{
    QString msg = format.arg(sender).arg(processMessage(message));
    bool blue = msg.startsWith("! ");
    bool magenta = msg.startsWith("* ");

    QString time = QTime::currentTime().toString();
    msg = QString("[%1] %2").arg(time).arg(msg);

    if (blue)
        msg = QString("<span style='color:darkblue'>%1</span>").arg(msg);
    else if (magenta)
        msg = QString("<span style='color:darkmagenta'>%1</span>").arg(msg);
    append(msg);
}
