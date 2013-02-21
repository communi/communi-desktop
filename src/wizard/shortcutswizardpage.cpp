/*
* Copyright (C) 2008-2013 Communi authors
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
    setPixmap(QWizard::LogoPixmap, QPixmap(":/resources/oxygen/64x64/actions/forward.png"));
    ui.treeWidget->setItemDelegate(new ItemDelegate(ui.treeWidget));
    ui.treeWidget->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui.treeWidget->expandItem(ui.treeWidget->topLevelItem(0));
    ui.treeWidget->expandItem(ui.treeWidget->topLevelItem(1));

    updateUi();
}

QHash<int, QString> ShortcutsWizardPage::shortcuts() const
{
    QHash<int, QString> shortcuts;
    shortcuts.insert(Settings::NextView, ui.treeWidget->topLevelItem(0)->text(Shortcut));
    shortcuts.insert(Settings::PreviousView, ui.treeWidget->topLevelItem(1)->text(Shortcut));
    shortcuts.insert(Settings::NextUnreadView, ui.treeWidget->topLevelItem(2)->text(Shortcut));
    shortcuts.insert(Settings::PreviousUnreadView, ui.treeWidget->topLevelItem(3)->text(Shortcut));
    shortcuts.insert(Settings::ExpandView, ui.treeWidget->topLevelItem(4)->text(Shortcut));
    shortcuts.insert(Settings::CollapseView, ui.treeWidget->topLevelItem(5)->text(Shortcut));
    shortcuts.insert(Settings::MostActiveView, ui.treeWidget->topLevelItem(6)->text(Shortcut));
    shortcuts.insert(Settings::SearchView, ui.treeWidget->topLevelItem(7)->text(Shortcut));
    return shortcuts;
}

void ShortcutsWizardPage::setShortcuts(const QHash<int, QString>& shortcuts)
{
    ui.treeWidget->topLevelItem(0)->setText(Shortcut, shortcuts.value(Settings::NextView));
    ui.treeWidget->topLevelItem(1)->setText(Shortcut, shortcuts.value(Settings::PreviousView));
    ui.treeWidget->topLevelItem(2)->setText(Shortcut, shortcuts.value(Settings::NextUnreadView));
    ui.treeWidget->topLevelItem(3)->setText(Shortcut, shortcuts.value(Settings::PreviousUnreadView));
    ui.treeWidget->topLevelItem(4)->setText(Shortcut, shortcuts.value(Settings::ExpandView));
    ui.treeWidget->topLevelItem(5)->setText(Shortcut, shortcuts.value(Settings::CollapseView));
    ui.treeWidget->topLevelItem(6)->setText(Shortcut, shortcuts.value(Settings::MostActiveView));
    ui.treeWidget->topLevelItem(7)->setText(Shortcut, shortcuts.value(Settings::SearchView));
}

void ShortcutsWizardPage::updateUi()
{
    //bool hasCurrent = ui.treeView->currentIndex().isValid();
    //ui.toolButtonEdit->setEnabled(hasCurrent);
    //ui.toolButtonRemove->setEnabled(hasCurrent);
}
