/*
  Copyright (C) 2008-2014 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

    const QString header("<tr><th align='left' colspan='3'><h3>%1</h3></th></tr>");
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
    shortcuts += row.arg(tr("Auto complete:"), QKeySequence("Tab").toString(QKeySequence::NativeText));
    shortcuts += row.arg(tr("Auto complete backwards:"), QKeySequence("Shift+Tab").toString(QKeySequence::NativeText));
    shortcuts += "<tr/>";
    shortcuts += row.arg(tr("Find:"), QKeySequence("Ctrl+F").toString(QKeySequence::NativeText));
    shortcuts += row.arg(tr("Search views:"), QKeySequence("Ctrl+S").toString(QKeySequence::NativeText));
    shortcuts += row.arg(tr("Search users:"), QKeySequence("Ctrl+U").toString(QKeySequence::NativeText));
    shortcuts += "</table>";

    QString commands;
    commands += "<table>";
    commands += header.arg(tr("Commands"));
    commands += "<tr/>";
    commands += row.arg("/AWAY", "(&lt;reason&gt;)");
    commands += row.arg("/CLEAR", "");
    commands += row.arg("/CLOSE", "");
    commands += row.arg("/INVITE", "&lt;user&gt; (&lt;channel&gt;)");
    commands += row.arg("/JOIN", "&lt;channel&gt; (&lt;key&gt;)");
    commands += row.arg("/KICK", "(&lt;channel&gt;) &lt;user&gt; (&lt;reason&gt;)");
    commands += row.arg("/ME", "&lt;message&gt;");
    commands += row.arg("/MODE", "(&lt;channel/user&gt;) (&lt;mode&gt;) (&lt;arg&gt;)");
    commands += row.arg("/MSG", "&lt;user/channel> &lt;message&gt;");
    commands += row.arg("/NAMES", "(&lt;channel&gt;)");
    commands += row.arg("/NICK", "&lt;nick&gt;");
    commands += row.arg("/NOTICE", "&lt;channel/user&gt; &lt;message&gt;");
    commands += row.arg("/PART", "(&lt;channel&gt;) (&lt;message&gt;)");
    commands += row.arg("/QUERY", "&lt;user&gt;");
    commands += row.arg("/QUIT", "(&lt;message&gt;)");
    commands += row.arg("/QUOTE", "&lt;command&gt; (&lt;parameters&gt;)");
    commands += row.arg("/TOPIC", "(&lt;channel&gt;) (&lt;topic&gt;)");
    commands += row.arg("/WHOIS", "&lt;user&gt;");
    commands += row.arg("/WHOWAS", "&lt;user&gt;");

    QString table("<table><td>%1</td><td width='25'></td><td>%2</td></table>");
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
