/****************************************************************************
 *  macdocktile.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <gorthauer87@ya.ru>
 *
 ***************************************************************************
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
*****************************************************************************/

#ifndef MACDOCKTILE_H
#define MACDOCKTILE_H

#include <qtdockprovider.h>

class QtMacDockTilePrivate;
class QtMacDockTile : public QtDockProvider
{
    Q_OBJECT
public:
    QtMacDockTile(QObject *parent = 0);
    virtual ~QtMacDockTile();
    virtual bool isUsable() const;
    virtual void setMenu(QMenu *menu);
    virtual void setBadge(const QString &badge);
    virtual void setProgress(int percents);
    virtual void alert(bool on);
private:
    bool m_isAlert;
};

#endif // MACDOCKTILE_H
