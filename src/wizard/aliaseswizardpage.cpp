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

#include "aliaseswizardpage.h"
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QComboBox>

enum Columns
{
    Alias,
    Command
};

class SortFilterProxyModel : public QSortFilterProxyModel
{
public:
    SortFilterProxyModel(QObject* parent = 0) : QSortFilterProxyModel(parent)
    {
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
    {
        QModelIndex aliasIndex = sourceModel()->index(sourceRow, Alias, sourceParent);
        QModelIndex commandIndex = sourceModel()->index(sourceRow, Command, sourceParent);
        if (!aliasIndex.isValid() || !commandIndex.isValid()) // the column may not exist
            return true;
        QString alias = aliasIndex.data(filterRole()).toString();
        QString command = commandIndex.data(filterRole()).toString();
        return alias.contains(filterRegExp()) || command.contains(filterRegExp());
    }
};

class CloseDelegate : public QStyledItemDelegate
{
public:
    CloseDelegate(QObject* parent = 0) : QStyledItemDelegate(parent)
    {
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        const_cast<QStyleOptionViewItem&>(option).decorationPosition = QStyleOptionViewItem::Right;
        QStyledItemDelegate::paint(painter, option, index);
    }
};

AliasesWizardPage::AliasesWizardPage(QWidget* parent) : QWizardPage(parent)
{
    ui.setupUi(this);
    setPixmap(QWizard::LogoPixmap, QPixmap(":/resources/oxygen/64x64/actions/forward.png"));

    proxyModel = new SortFilterProxyModel(ui.treeView);
    sourceModel = new QStandardItemModel(proxyModel);
    proxyModel->setSourceModel(sourceModel);
    ui.treeView->setModel(proxyModel);
    ui.treeView->setItemDelegateForColumn(2, new CloseDelegate(ui.treeView));

    connect(ui.toolButtonAdd, SIGNAL(clicked()), this, SLOT(addAlias()));
    connect(ui.treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onClicked(QModelIndex)));

    connect(ui.lineEditFilter, SIGNAL(textEdited(QString)), proxyModel, SLOT(setFilterWildcard(QString)));
}

QMap<QString, QString> AliasesWizardPage::aliases() const
{
    QMap<QString, QString> aliases;
    for (int i = 0; i < sourceModel->rowCount(); ++i)
    {
        QString alias = sourceModel->item(i, Alias)->text();
        QString command = sourceModel->item(i, Command)->text();
        if (!alias.isEmpty() && !command.isEmpty())
            aliases[alias] = command;
    }
    return aliases;
}

void AliasesWizardPage::setAliases(const QMap<QString, QString>& aliases)
{
    sourceModel->clear();
    sourceModel->setHorizontalHeaderLabels(QStringList() << tr("Alias") << tr("Command") << QString());

    QMapIterator<QString, QString> it(aliases);
    while (it.hasNext())
    {
        it.next();
        QString alias = it.key();
        QString command = it.value();
        if (!alias.isEmpty() && !command.isEmpty())
            addRow(alias, command);
    }

    ui.treeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui.treeView->header()->setResizeMode(1, QHeaderView::Stretch);
    ui.treeView->header()->setResizeMode(2, QHeaderView::ResizeToContents);
}

void AliasesWizardPage::addAlias()
{
    ui.lineEditFilter->clear();
    QModelIndex index = addRow(QString(), QString());
    ui.treeView->setCurrentIndex(index);
    ui.treeView->edit(index);
}

QModelIndex AliasesWizardPage::addRow(const QString& alias, const QString& command)
{
    QStandardItem* aliasItem = new QStandardItem(alias);
    QStandardItem* commandItem = new QStandardItem(command);
    QStandardItem* closeItem = new QStandardItem(QPixmap(":/resources/icons/buttons/close.png"), QString());
    closeItem->setFlags(closeItem->flags() & ~Qt::ItemIsEditable);
    sourceModel->appendRow(QList<QStandardItem*>() << aliasItem << commandItem << closeItem);
    return proxyModel->mapFromSource(aliasItem->index());
}

void AliasesWizardPage::onClicked(const QModelIndex& index)
{
    if (index.column() == 2)
        qDeleteAll(sourceModel->takeRow(proxyModel->mapToSource(index).row()));
}
