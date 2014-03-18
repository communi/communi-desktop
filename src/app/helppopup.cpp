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

#include "helppopup.h"
#include <QMouseEvent>
#include <QShortcut>
#include <QToolTip>
#include <QStyle>

HelpPopup::HelpPopup(QWidget* parent) : QLabel(parent)
{
    setForegroundRole(QPalette::ToolTipText);
    setBackgroundRole(QPalette::ToolTipBase);
    setPalette(QToolTip::palette());
    ensurePolished();
    setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, this));
    setFrameStyle(QFrame::NoFrame);
    setAlignment(Qt::AlignLeft);
    setIndent(1);
    setWindowOpacity(style()->styleHint(QStyle::SH_ToolTipLabel_Opacity, 0, this) / 255.0);
    setMouseTracking(true);

    setWindowFlags(Qt::Popup);
    setAttribute(Qt::WA_DeleteOnClose);

    QShortcut* shortcut = new QShortcut(QKeySequence("Esc"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));
}

HelpPopup::~HelpPopup()
{
}

void HelpPopup::popup()
{
#ifdef Q_OS_MAC
    QString control(tr("Cmd+%1"));
    QString navigate(tr("Cmd+Alt+%1"));
    QString nextActive(tr("Shift+Cmd+Alt+%1"));
#else
    QString control(tr("Ctrl+%1"));
    QString navigate(tr("Alt+%1"));
    QString nextActive(tr("Shift+Alt+%1"));
#endif

    QString html;
    html += tr("<table>");
    html += tr("<tr><td><table>");
    html += tr("<tr><th align='left' colspan='2'><h3>Shortcuts</h3></th></tr>");
    html += tr("<tr/>");
    html += tr("<tr><th align='left'>Connect:</th><td><pre>%1</pre></td></tr>").arg(QKeySequence(QKeySequence::New).toString());
    html += tr("<tr><th align='left'>Settings:</th><td><pre>%1</pre></td></tr>").arg(QKeySequence(QKeySequence::Preferences).toString());
    html += tr("<tr><th align='left'>Close:</th><td><pre>%1</pre></td></tr>").arg(QKeySequence(QKeySequence::Close).toString());
    html += tr("<tr/>");
    html += tr("<tr><th align='left'>Next view:</th><td><pre>%1</pre></td></tr>").arg(navigate.arg(tr("Down")));
    html += tr("<tr><th align='left'>Previous view:</th><td><pre>%1</pre></td></tr>").arg(navigate.arg(tr("Up")));
    html += tr("<tr><th align='left'>Next active view:</th><td><pre>%1</pre></td></tr>").arg(nextActive.arg(tr("Down")));
    html += tr("<tr><th align='left'>Previous active view:</th><td><pre>%1</pre></td></tr>").arg(navigate.arg(tr("Up")));
    html += tr("<tr><th align='left'>Most live view:</th><td><pre>%1</pre></td></tr>").arg(control.arg("L"));
    html += tr("<tr><th align='left'>Expand view:</th><td><pre>%1</pre></td></tr>").arg(navigate.arg(tr("Right")));
    html += tr("<tr><th align='left'>Collapse view:</th><td><pre>%1</pre></td></tr>").arg(navigate.arg(tr("Left")));
    html += tr("<tr/>");
    html += tr("<tr><th align='left'>Find:</th><td><pre>%1</pre></td></tr>").arg(control.arg("F"));
    html += tr("<tr><th align='left'>Search views:</th><td><pre>%1</pre></td></tr>").arg(control.arg("S"));
    html += tr("<tr><th align='left'>Search users:</th><td><pre>%1</pre></td></tr>").arg(control.arg("U"));
    html += tr("</table></td>");

    // TODO:
//    html += tr("<td><table>");
//    html += tr("<tr><th align='left' colspan='2'><h3>Commands</h3></th></tr>");
//    html += tr("<tr/>");
//    CommandParser parser;
//    QStringList commands;
//    commands << "ACTION" << "AWAY" << "INVITE" << "JOIN" << "KICK" << "ME" << "MODE"
//             << "MSG" << "NICK" << "NOTICE" << "PART" << "QUERY" << "TOPIC" << "WHOIS";
//    foreach (const QString& command, commands) {
//        QString syntax = QStringList(parser.visualSyntax(command).split(" ").mid(1)).join(" ");
//        syntax.replace("<", "&lt;");
//        syntax.replace(">", "&gt;");
//        html += tr("<tr><th align='left'>/%1</th><td>%2</td></tr>").arg(command, syntax);
//    }
//    html += tr("</table></td></tr>");
    html += tr("</table>");
    setText(html);

    adjustSize();
    QRect rect = geometry();
    rect.moveCenter(parentWidget()->geometry().center());
    move(rect.topLeft());

    show();
    raise();
    setFocus();
    activateWindow();
}

void HelpPopup::keyPressEvent(QKeyEvent* event)
{
    QLabel::keyPressEvent(event);
    close();
}

void HelpPopup::mousePressEvent(QMouseEvent* event)
{
    QLabel::mousePressEvent(event);
    close();
}
