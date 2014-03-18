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
    QString next = tr("Alt+Tab");
    QString previous = tr("Shift+Alt+Tab");
    QString navigate(tr("Ctrl+Alt+%1"));
    QString active(tr("Shift+Ctrl+Alt+%1"));
#else
    QString next = tr("Ctrl+Tab");
    QString previous = tr("Shift+Ctrl+Tab");
    QString navigate(tr("Alt+%1"));
    QString active(tr("Shift+Alt+%1"));
#endif

    const QString header("<tr><th align='left' colspan='2'><h3>%1</h3></th></tr>");
    const QString row("<tr><th align='left'>%1</th><td>&nbsp;</td><td><pre>%2</pre></td></tr>");

    QString shortcuts;
    shortcuts += "<table>";
    shortcuts += header.arg(tr("Shortcuts"));
    shortcuts += "<tr/>";
    shortcuts += row.arg(tr("Next view:"), QKeySequence(navigate.arg(tr("Down"))).toString(QKeySequence::NativeText));
    shortcuts += row.arg(tr("Previous view:"), QKeySequence(navigate.arg(tr("Up"))).toString(QKeySequence::NativeText));
    shortcuts += "<tr/>";
    shortcuts += row.arg(tr("Next active view:"), QKeySequence(active.arg(tr("Down"))).toString(QKeySequence::NativeText));
    shortcuts += row.arg(tr("Previous active view:"), QKeySequence(active.arg(tr("Up"))).toString(QKeySequence::NativeText));
    shortcuts += row.arg(tr("Most live view:"), QKeySequence("Ctrl+L").toString(QKeySequence::NativeText));
    shortcuts += "<tr/>";
    shortcuts += row.arg(tr("Expand view:"), QKeySequence(navigate.arg(tr("Right"))).toString(QKeySequence::NativeText));
    shortcuts += row.arg(tr("Collapse view:"), QKeySequence(navigate.arg(tr("Left"))).toString(QKeySequence::NativeText));
    shortcuts += "<tr/>";
    shortcuts += row.arg(tr("Next split:"), QKeySequence(next).toString(QKeySequence::NativeText));
    shortcuts += row.arg(tr("Previous split:"), QKeySequence(previous).toString(QKeySequence::NativeText));
    shortcuts += "<tr/>";
    shortcuts += row.arg(tr("Find:"), QKeySequence("Ctrl+F").toString(QKeySequence::NativeText));
    shortcuts += row.arg(tr("Search views:"), QKeySequence("Ctrl+S").toString(QKeySequence::NativeText));
    shortcuts += row.arg(tr("Search users:"), QKeySequence("Ctrl+U").toString(QKeySequence::NativeText));
    shortcuts += "</table>";

    QString commands;
    commands += "<table>";
    commands += header.arg(tr("Commands"));
    commands += "<tr/>";
    commands += row.arg("CLEAR", "");
    commands += row.arg("JOIN", "JOIN &lt;channel&gt; (&lt;key&gt;)");
    commands += row.arg("ME", "&lt;message...&gt;");
    commands += row.arg("MSG", "&lt;user/channel> &lt;message...&gt;");
    commands += row.arg("NICK", "&lt;nick&gt;");
    commands += row.arg("NOTICE", "&lt;channel/user&gt; &lt;message...&gt;");
    commands += row.arg("PART", "(&lt;channel&gt;) (&lt;message...&gt;)");
    commands += row.arg("QUERY", "&lt;user&gt;");
    commands += row.arg("TOPIC", "(&lt;channel&gt;) (&lt;topic...&gt;)");

    QString table("<table><td>%1</td><td>&nbsp;</td><td>%2</td></table>");
    setText(table.arg(shortcuts, commands));

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
