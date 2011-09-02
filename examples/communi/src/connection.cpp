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

#include "connection.h"

Connection::Connection() :
    name(),
    host(),
    port(6667),
    nick(),
    real(),
    pass(),
    secure(false)
{
}

Connection::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

bool Connection::operator==(const Connection& other) const
{
    return name == other.name;
}

bool Connection::operator!=(const Connection& other) const
{
    return name != other.name;
}
