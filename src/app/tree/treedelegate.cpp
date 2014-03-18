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

#include "treedelegate.h"
#include "treerole.h"
#include <QStyleOptionViewItem>
#include <QStyleOptionHeader>
#include <QStylePainter>
#include <QApplication>
#include <QHeaderView>
#include <QTreeView>
#include <QPalette>
#include <QPainter>
#include <QPointer>
#include <QLabel>
#include <QStyle>
#include <QColor>

class TreeHeader : public QFrame
{
    Q_OBJECT

public:
    TreeHeader(QWidget* parent = 0) : QFrame(parent)
    {
        d.state = QStyle::State_None;
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_NoSystemBackground);
        setVisible(false);
    }

    static TreeHeader* instance(QWidget* parent = 0)
    {
        static QHash<QWidget*, TreeHeader*> headers;
        QWidget* window = parent ? parent->window() : 0;
        TreeHeader* header = headers.value(window);
        if (!header) {
            header = new TreeHeader(window);
            headers.insert(window, header);
        }
        return header;
    }

    void setText(const QString& text) { d.text = text; }
    void setIcon(const QIcon& icon) { d.icon = icon; }
    void setState(QStyle::State state) { d.state = state; }

protected:
    void paintEvent(QPaintEvent*)
    {
        QStyleOptionHeader option;
        option.init(this);
#ifdef Q_OS_WIN
        option.rect.adjust(0, 0, 0, 1);
#endif
        option.state = (d.state | QStyle::State_Raised | QStyle::State_Horizontal);
        option.icon = d.icon;
        option.text = d.text;
        option.position = QStyleOptionHeader::OnlyOneSection;
        QStylePainter painter(this);
        painter.drawControl(QStyle::CE_Header, option);
    }

private:
    struct Private {
        QIcon icon;
        QString text;
        QStyle::State state;
    } d;
};

class TreeBadge : public QLabel
{
    Q_OBJECT

public:
    TreeBadge(QWidget* parent = 0) : QLabel(parent)
    {
        d.num = 0;
        d.hilite = false;
        setAlignment(Qt::AlignCenter);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_NoSystemBackground);
        setVisible(false);
    }

    static TreeBadge* instance(QWidget* parent = 0)
    {
        static QHash<QWidget*, TreeBadge*> badges;
        QWidget* window = parent ? parent->window() : 0;
        TreeBadge* badge = badges.value(window);
        if (!badge) {
            badge = new TreeBadge(window);
            badges.insert(window, badge);
        }
        return badge;
    }

    void setHighlighted(int hilite) { d.hilite = hilite; }

    void setNum(int num)
    {
        d.num = num;
        QString txt;
        if (d.num > 999)
            txt = QLatin1String("...");
        else
            txt = fontMetrics().elidedText(QString::number(d.num), Qt::ElideRight, width());
        setText(txt);
    }

protected:
    void paintEvent(QPaintEvent*)
    {
        QPainter painter(this);
        drawBackground(&painter);
        QRect cr = contentsRect();
        cr.adjust(margin(), margin(), -margin(), -margin());
        style()->drawItemText(&painter, cr, alignment(), palette(), isEnabled(), text(), foregroundRole());
    }

    void drawBackground(QPainter* painter)
    {
        QStyleOptionFrame frame;
        frame.init(this);
        int frameShape  = frameStyle() & QFrame::Shape_Mask;
        int frameShadow = frameStyle() & QFrame::Shadow_Mask;
        frame.frameShape = Shape(int(frame.frameShape) | frameShape);
        frame.rect = frameRect();
        switch (frameShape) {
            case QFrame::Box:
            case QFrame::HLine:
            case QFrame::VLine:
            case QFrame::StyledPanel:
            case QFrame::Panel:
                frame.lineWidth = lineWidth();
                frame.midLineWidth = midLineWidth();
                break;
            default:
                frame.lineWidth = frameWidth();
                break;
        }
        if (frameShadow == Sunken)
            frame.state |= QStyle::State_Sunken;
        else if (frameShadow == Raised)
            frame.state |= QStyle::State_Raised;
        if (d.hilite)
            frame.state |= QStyle::State_On;
        style()->drawPrimitive(QStyle::PE_Widget, &frame, painter, this);
        style()->drawControl(QStyle::CE_ShapedFrame, &frame, painter, this);
    }

private:
    struct Private {
        int num;
        bool hilite;
    } d;
};

TreeDelegate::TreeDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

QSize TreeDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize sz = QStyledItemDelegate::sizeHint(option, index);
    if (!index.parent().isValid()) {
        // QMacStyle wants a QHeaderView that is a child of QTreeView :/
        QTreeView tree;
        QStyleOptionHeader opt;
        QSize ss = qApp->style()->sizeFromContents(QStyle::CT_HeaderSection, &opt, QSize(), tree.header());
        if (ss.isValid())
            return ss;
    }
    return sz;
}

void TreeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    bool hilite = index.data(TreeRole::Highlight).toBool();
    if (hilite)
        const_cast<QStyleOptionViewItem&>(option).state |= QStyle::State_On;

    bool active = index.data(TreeRole::Active).toBool();
    if (!hilite && !active)
        const_cast<QStyleOptionViewItem&>(option).state |= QStyle::State_Off;

    if (!index.parent().isValid()) {
        TreeHeader* header = TreeHeader::instance(const_cast<QWidget*>(option.widget));
        header->setText(index.data(Qt::DisplayRole).toString());
        header->setIcon(index.data(Qt::DecorationRole).value<QIcon>());
        header->setState(option.state);
        header->setGeometry(option.rect);
        painter->translate(option.rect.topLeft());
        header->render(painter);
        painter->translate(-option.rect.topLeft());
    } else {
        QStyledItemDelegate::paint(painter, option, index);

        int num = index.data(TreeRole::Badge).toInt();
        if (num > 0) {
            TreeBadge* badge = TreeBadge::instance(const_cast<QWidget*>(option.widget));
            badge->setNum(num);
            badge->setHighlighted(hilite);

            badge->setGeometry(option.rect);
            painter->translate(option.rect.topLeft());
            badge->render(painter);
            painter->translate(-option.rect.topLeft());
        }
    }
}

void TreeDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    if (index.parent().isValid())
        option->backgroundBrush = Qt::transparent;
}

#include "treedelegate.moc"
