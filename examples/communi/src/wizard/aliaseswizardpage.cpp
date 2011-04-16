/*
* Copyright (C) 2008-2010 J-P Nurmi jpnurmi@gmail.com
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
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
* $Id$
*/

#include "aliaseswizardpage.h"
#include <QSortFilterProxyModel>
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

AliasesWizardPage::AliasesWizardPage(QWidget* parent) : QWizardPage(parent)
{
    ui.setupUi(this);
    setPixmap(QWizard::LogoPixmap, QPixmap(":/resources/oxygen/64x64/actions/forward.png"));

    proxyModel = new SortFilterProxyModel(ui.treeView);
    sourceModel = new QStandardItemModel(proxyModel);
    proxyModel->setSourceModel(sourceModel);
    ui.treeView->setModel(proxyModel);

    connect(ui.toolButtonAdd, SIGNAL(clicked()), this, SLOT(addAlias()));
    connect(ui.toolButtonEdit, SIGNAL(clicked()), this, SLOT(editAlias()));
    connect(ui.toolButtonRemove, SIGNAL(clicked()), this, SLOT(removeAlias()));

    connect(ui.lineEditFilter, SIGNAL(textEdited(QString)), proxyModel, SLOT(setFilterWildcard(QString)));
    connect(ui.treeView->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(updateUi()));
    updateUi();
}

QMap<QString, QString> AliasesWizardPage::aliases() const
{
    QMap<QString, QString> aliases;
    for (int i = 0; i < sourceModel->rowCount(); ++i)
    {
        QStandardItem* aliasItem = sourceModel->item(i, Alias);
        QStandardItem* commandItem = sourceModel->item(i, Command);
        aliases[aliasItem->text()] = commandItem->text();
    }
    return aliases;
}

void AliasesWizardPage::setAliases(const QMap<QString, QString>& aliases)
{
    sourceModel->clear();
    sourceModel->setHorizontalHeaderLabels(QStringList() << tr("Alias") << tr("Command"));

    QMapIterator<QString, QString> it(aliases);
    while (it.hasNext())
    {
        it.next();
        QStandardItem* aliasItem = new QStandardItem(it.key());
        QStandardItem* commandItem = new QStandardItem(it.value());
        sourceModel->appendRow(QList<QStandardItem*>() << aliasItem << commandItem);
    }
}

void AliasesWizardPage::addAlias()
{
    ui.lineEditFilter->clear();
    QStandardItem* aliasItem = new QStandardItem;
    QStandardItem* commandItem = new QStandardItem;
    sourceModel->appendRow(QList<QStandardItem*>() << aliasItem << commandItem);
    QModelIndex index = proxyModel->mapFromSource(aliasItem->index());
    ui.treeView->setCurrentIndex(index);
    ui.treeView->edit(index);
}

void AliasesWizardPage::editAlias()
{
    QModelIndex index = ui.treeView->currentIndex();
    index = proxyModel->mapFromSource(index);
    ui.treeView->edit(index.sibling(index.row(), Alias));
}

void AliasesWizardPage::removeAlias()
{
    QModelIndex index = ui.treeView->currentIndex();
    qDeleteAll(sourceModel->takeRow(proxyModel->mapToSource(index).row()));
}

void AliasesWizardPage::updateUi()
{
    bool hasCurrent = ui.treeView->currentIndex().isValid();
    ui.toolButtonEdit->setEnabled(hasCurrent);
    ui.toolButtonRemove->setEnabled(hasCurrent);
}
