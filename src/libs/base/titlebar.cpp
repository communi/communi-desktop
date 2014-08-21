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

#include "titlebar.h"
#include "messageformatter.h"
#include <QStyleOptionHeader>
#include <QPropertyAnimation>
#include <QStylePainter>
#include <IrcTextFormat>
#include <QApplication>
#include <QMouseEvent>
#include <QHeaderView>
#include <IrcCommand>
#include <IrcChannel>
#include <QTreeView>
#include <QStyle>
#include <QMenu>

class TitleMenu : public QMenu
{
public:
    TitleMenu(QWidget* parent) : QMenu(parent) { }

protected:
    void moveEvent(QMoveEvent*)
    {
        // TODO: looks scary - any better ideas?
        if (QWidget* parent = parentWidget()) {
            QRect rect = geometry();
            rect.moveTopRight(parent->mapToGlobal(parent->rect().bottomRight()));
#ifdef Q_OS_MAC
            rect.translate(0, -2); // TODO
#endif
            setGeometry(rect);
        }
    }
};

TitleBar::TitleBar(QWidget* parent) : QLabel(parent)
{
    d.buffer = 0;
    d.baseOffset = -1;
    d.editor = new QTextEdit(this);
    d.formatter = new MessageFormatter(this);

    setWordWrap(true);
    setOpenExternalLinks(true);
    setTextFormat(Qt::RichText);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

#ifdef Q_OS_MAC
    QFont font;
    font.setPointSize(11.0);
    setFont(font);
#endif

    d.editor->setVisible(false);
    d.editor->setAcceptRichText(false);
    d.editor->setTabChangesFocus(true);
    d.editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d.editor->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d.editor->installEventFilter(this);

    d.menuButton = new QToolButton(this);
    d.menuButton->setObjectName("menu");
    d.menuButton->setMenu(new TitleMenu(d.menuButton));
    d.menuButton->setPopupMode(QToolButton::InstantPopup);
    d.menuButton->setFocusPolicy(Qt::NoFocus);
    d.menuButton->adjustSize();

    adjustSize();
    relayout();
}

QMenu* TitleBar::menu() const
{
    return d.menuButton->menu();
}

QSize TitleBar::minimumSizeHint() const
{
    // QMacStyle wants a QHeaderView that is a child of QTreeView :/
    QTreeView tree;
    QStyleOptionHeader option;
    return style()->sizeFromContents(QStyle::CT_HeaderSection, &option, QSize(), tree.header());
}

int TitleBar::heightForWidth(int width) const
{
    return qMax(minimumSizeHint().height(), QLabel::heightForWidth(width));
}

int TitleBar::baseOffset() const
{
    return d.baseOffset;
}

int TitleBar::offset() const
{
    return height() - minimumSizeHint().height() - d.baseOffset;
}

void TitleBar::setOffset(int offset)
{
    resize(width(), minimumSizeHint().height() + offset);
}

QString TitleBar::styleSheet() const
{
    return d.css;
}

void TitleBar::setStyleSheet(const QString& css)
{
    d.css = css;
    refresh();
}

void TitleBar::expand()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "offset");
    animation->setDuration(50);
    animation->setStartValue(offset());
    animation->setEndValue(qMax(0, QLabel::heightForWidth(width()) - minimumSizeHint().height()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void TitleBar::collapse()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "offset");
    animation->setDuration(50);
    animation->setStartValue(offset());
    animation->setEndValue(0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

IrcBuffer* TitleBar::buffer() const
{
    return d.buffer;
}

void TitleBar::setBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        if (d.buffer) {
            IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
            if (channel) {
                disconnect(channel, SIGNAL(destroyed(IrcChannel*)), this, SLOT(cleanup()));
                disconnect(channel, SIGNAL(topicChanged(QString)), this, SLOT(refresh()));
                disconnect(channel, SIGNAL(modeChanged(QString)), this, SLOT(refresh()));
            } else {
                disconnect(d.buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(cleanup()));
            }
            disconnect(d.buffer, SIGNAL(titleChanged(QString)), this, SLOT(refresh()));
        }
        d.buffer = buffer;
        if (d.buffer) {
            IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
            if (channel) {
                connect(channel, SIGNAL(destroyed(IrcChannel*)), this, SLOT(cleanup()));
                connect(channel, SIGNAL(topicChanged(QString)), this, SLOT(refresh()));
                connect(channel, SIGNAL(modeChanged(QString)), this, SLOT(refresh()));
            } else {
                connect(d.buffer, SIGNAL(destroyed(IrcBuffer*)), this, SLOT(cleanup()));
            }
            connect(d.buffer, SIGNAL(titleChanged(QString)), this, SLOT(refresh()));
        }
        collapse();
        refresh();
    }
}

QString TitleBar::topic() const
{
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    if (channel)
        return channel->topic();
    return QString();
}

void TitleBar::setTopic(const QString& topic)
{
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    if (channel) {
        if (channel->topic() != topic)
            channel->sendCommand(IrcCommand::createTopic(channel->title(), topic));
    }
}

bool TitleBar::event(QEvent* event)
{
    switch (event->type()) {
    case QEvent::Enter:
        if (!d.editor->isVisible())
            expand();
        break;
    case QEvent::Leave:
        if (!d.editor->isVisible())
            collapse();
        break;
    default:
        break;
    }
    return QLabel::event(event);
}

bool TitleBar::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    switch (event->type()) {
    case QEvent::Hide:
        if (!underMouse())
            collapse();
        break;
    case QEvent::FocusOut:
        d.editor->hide();
        break;
    case QEvent::KeyPress: {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            setTopic(d.editor->toPlainText());
            d.editor->hide();
        } else if (keyEvent->key() == Qt::Key_Escape) {
            d.editor->hide();
        }
        break;
    }
    default:
        break;
    }
    return false;
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    edit();
    d.editor->setTextCursor(d.editor->cursorForPosition(event->pos()));
}

void TitleBar::paintEvent(QPaintEvent* event)
{
    QStyleOptionHeader option;
    option.init(this);
#ifdef Q_OS_WIN
    option.rect.adjust(0, 0, 0, 1);
#endif
    option.state = (QStyle::State_Raised | QStyle::State_Horizontal);
    option.position = QStyleOptionHeader::OnlyOneSection;
    QStylePainter painter(this);
    painter.drawControl(QStyle::CE_HeaderSection, option);

    QLabel::paintEvent(event);
}

void TitleBar::resizeEvent(QResizeEvent* event)
{
    if (d.baseOffset == -1)
        d.baseOffset = qMax(0, QLabel::heightForWidth(-1) - minimumSizeHint().height());

    relayout();
    QLabel::resizeEvent(event);
    emit offsetChanged(offset());
}

void TitleBar::relayout()
{
    QRect r = d.menuButton->rect();
    r.moveTopRight(rect().topRight());
    d.menuButton->setGeometry(r);

    QStyleOptionHeader option;
    option.initFrom(this);
    option.rect.setRight(r.left() - 1);
    QRect ser = style()->subElementRect(QStyle::SE_HeaderLabel, &option, this);
    setContentsMargins(ser.x(), topMargin() + ser.y(), width() - ser.x() - ser.width(), height() - ser.y() - ser.height());
}

void TitleBar::cleanup()
{
    d.buffer = 0;
    refresh();
}

void TitleBar::refresh()
{
    clear();
    QString title = d.buffer ? d.buffer->title() : QString();
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    QString topic = channel ? channel->topic() : QString();
    if (!topic.isEmpty())
        title = tr("%1: %2").arg(title, d.formatter->formatText(topic, Qt::RichText));
    setText(title);
    foreach (QTextDocument* doc, findChildren<QTextDocument*>())
        doc->setDefaultStyleSheet(d.css);
}

void TitleBar::edit()
{
    IrcChannel* channel = qobject_cast<IrcChannel*>(d.buffer);
    if (channel) {
        d.editor->setPlainText(channel->topic());

        // keep the text visually in place...
        d.editor->document()->setIndentWidth(3);
        d.editor->document()->setDocumentMargin(1);
        QTextCursor cursor(d.editor->document());
        QTextBlockFormat format = cursor.blockFormat();
        format.setIndent(1);
        cursor.setBlockFormat(format);

        d.editor->resize(size()); // TODO: layout?
        d.editor->setFocus();
        d.editor->show();
    }
}

int TitleBar::topMargin() const
{
#ifdef Q_OS_MAC
    return fontMetrics().boundingRect(0, 0, width(), minimumSizeHint().height(), Qt::AlignCenter, "...").top() + 1;
#else
    return 0;
#endif
}
