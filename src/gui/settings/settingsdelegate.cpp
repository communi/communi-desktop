/*
* Copyright (C) 2008-2013 The Communi Project
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

#include "settingsdelegate.h"
#include <QFontComboBox>
#include <QComboBox>
#include <QDebug>

// SVG color keyword names provided by the World Wide Web Consortium
static const QStringList svgColorNames()
{
    return QStringList()
        << "aliceblue" << "antiquewhite" << "aqua" << "aquamarine" << "azure" << "beige" << "bisque"
        << "black" << "blanchedalmond" << "blue" << "blueviolet" << "brown" << "burlywood" << "cadetblue"
        << "chartreuse" << "chocolate" << "coral" << "cornflowerblue" << "cornsilk" << "crimson" << "cyan"
        << "darkblue" << "darkcyan" << "darkgoldenrod" << "darkgray" << "darkgreen" << "darkgrey"
        << "darkkhaki" << "darkmagenta" << "darkolivegreen" << "darkorange" << "darkorchid" << "darkred"
        << "darksalmon" << "darkseagreen" << "darkslateblue" << "darkslategray" << "darkslategrey"
        << "darkturquoise" << "darkviolet" << "deeppink" << "deepskyblue" << "dimgray" << "dimgrey"
        << "dodgerblue" << "firebrick" << "floralwhite" << "forestgreen" << "fuchsia" << "gainsboro"
        << "ghostwhite" << "gold" << "goldenrod" << "gray" << "grey" << "green" << "greenyellow"
        << "honeydew" << "hotpink" << "indianred" << "indigo" << "ivory" << "khaki" << "lavender"
        << "lavenderblush" << "lawngreen" << "lemonchiffon" << "lightblue" << "lightcoral" << "lightcyan"
        << "lightgoldenrodyellow" << "lightgray" << "lightgreen" << "lightgrey" << "lightpink"
        << "lightsalmon" << "lightseagreen" << "lightskyblue" << "lightslategray" << "lightslategrey"
        << "lightsteelblue" << "lightyellow" << "lime" << "limegreen" << "linen" << "magenta"
        << "maroon" << "mediumaquamarine" << "mediumblue" << "mediumorchid" << "mediumpurple"
        << "mediumseagreen" << "mediumslateblue" << "mediumspringgreen" << "mediumturquoise"
        << "mediumvioletred" << "midnightblue" << "mintcream" << "mistyrose" << "moccasin"
        << "navajowhite" << "navy" << "oldlace" << "olive" << "olivedrab" << "orange" << "orangered"
        << "orchid" << "palegoldenrod" << "palegreen" << "paleturquoise" << "palevioletred"
        << "papayawhip" << "peachpuff" << "peru" << "pink" << "plum" << "powderblue" << "purple" << "red"
        << "rosybrown" << "royalblue" << "saddlebrown" << "salmon" << "sandybrown" << "seagreen"
        << "seashell" << "sienna" << "silver" << "skyblue" << "slateblue" << "slategray" << "slategrey"
        << "snow" << "springgreen" << "steelblue" << "tan" << "teal" << "thistle" << "tomato"
        << "turquoise" << "violet" << "wheat" << "white" << "whitesmoke" << "yellow" << "yellowgreen";
}

SettingsDelegate::SettingsDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

QWidget* SettingsDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.data().type() == QVariant::Color) {
        QComboBox* comboBox = new QComboBox(parent);
        comboBox->addItems(svgColorNames());
        comboBox->setEditable(true);
        int i = 0;
        foreach (const QString& color, svgColorNames())
            comboBox->setItemData(i++, QColor(color), Qt::DecorationRole);
        return comboBox;
    } else if (index.data().type() == QVariant::Font) {
        return new QFontComboBox(parent);
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void SettingsDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (index.data().type() == QVariant::Color) {
        QComboBox* comboBox = static_cast<QComboBox*>(editor);
        comboBox->setEditText(index.data().toString());
    } else if (index.data().type() == QVariant::Font) {
        QFontComboBox* comboBox = static_cast<QFontComboBox*>(editor);
        comboBox->setCurrentFont(index.data().value<QFont>());
        comboBox->setEditText(comboBox->currentFont().toString());
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void SettingsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (index.data().type() == QVariant::Color) {
        QComboBox* comboBox = static_cast<QComboBox*>(editor);
        model->setData(index, QColor(comboBox->currentText()));
    } else if (index.data().type() == QVariant::Font) {
        QFontComboBox* comboBox = static_cast<QFontComboBox*>(editor);
        QFont font;
        if (font.fromString(comboBox->currentText()))
            model->setData(index, font);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
