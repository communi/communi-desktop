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

#include "shortcutswizardpage.h"
#include "settings.h"
#include <QStyledItemDelegate>

enum Columns {
    Description,
    Shortcut
};

class ItemDelegate : public QStyledItemDelegate
{
public:
    ItemDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) { }
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& opt, const QModelIndex& index) const {
        QWidget* editor = 0;
        if (index.column() == Shortcut)
            editor = QStyledItemDelegate::createEditor(parent, opt, index);
        return editor;
    }
};

ShortcutsWizardPage::ShortcutsWizardPage(QWidget* parent) : QWizardPage(parent)
{
    ui.setupUi(this);
    ui.treeWidget->setItemDelegate(new ItemDelegate(ui.treeWidget));
    ui.treeWidget->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui.treeWidget->expandItem(ui.treeWidget->topLevelItem(0));
    ui.treeWidget->expandItem(ui.treeWidget->topLevelItem(1));

    updateUi();
}

QHash<int, QString> ShortcutsWizardPage::shortcuts() const
{
    QHash<int, QString> shortcuts;
    shortcuts.insert(Settings::NextView, logicalShortcut(0));
    shortcuts.insert(Settings::PreviousView, logicalShortcut(1));
    shortcuts.insert(Settings::NextUnreadView, logicalShortcut(2));
    shortcuts.insert(Settings::PreviousUnreadView, logicalShortcut(3));
    shortcuts.insert(Settings::ExpandView, logicalShortcut(4));
    shortcuts.insert(Settings::CollapseView, logicalShortcut(5));
    shortcuts.insert(Settings::MostActiveView, logicalShortcut(6));
    shortcuts.insert(Settings::SearchView, logicalShortcut(7));
    return shortcuts;
}

void ShortcutsWizardPage::setShortcuts(const QHash<int, QString>& shortcuts)
{
    setVisualShortcut(0, shortcuts.value(Settings::NextView));
    setVisualShortcut(1, shortcuts.value(Settings::PreviousView));
    setVisualShortcut(2, shortcuts.value(Settings::NextUnreadView));
    setVisualShortcut(3, shortcuts.value(Settings::PreviousUnreadView));
    setVisualShortcut(4, shortcuts.value(Settings::ExpandView));
    setVisualShortcut(5, shortcuts.value(Settings::CollapseView));
    setVisualShortcut(6, shortcuts.value(Settings::MostActiveView));
    setVisualShortcut(7, shortcuts.value(Settings::SearchView));
}

void ShortcutsWizardPage::updateUi()
{
    //bool hasCurrent = ui.treeView->currentIndex().isValid();
    //ui.toolButtonEdit->setEnabled(hasCurrent);
    //ui.toolButtonRemove->setEnabled(hasCurrent);
}

QString ShortcutsWizardPage::logicalShortcut(int index) const
{
    //QString text = ui.treeWidget->topLevelItem(index)->text(Shortcut);
    //QString shortcut = QKeySequence(text).toString(QKeySequence::PortableText);
    QString shortcut = ui.treeWidget->topLevelItem(index)->text(Shortcut);
#ifdef Q_OS_MAC
    shortcut.replace("Cmd", "Ctrl");
#endif
    return shortcut;
}

void ShortcutsWizardPage::setVisualShortcut(int index, const QString& text)
{
    //QString shortcut = QKeySequence(text, QKeySequence::PortableText).toString(QKeySequence::NativeText);
    QString shortcut = text;
#ifdef Q_OS_MAC
    shortcut.replace("Ctrl", "Cmd");
#endif
    ui.treeWidget->topLevelItem(index)->setText(Shortcut, shortcut);
}
