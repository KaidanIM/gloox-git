/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/



#include "privacyitem.h"

namespace gloox
{

  PrivacyItem::PrivacyItem( const ItemType type, const ItemAction action,
                            const int packetType, const string& value )
    : m_type( type ), m_action( action ), m_packetType( packetType ),
    m_value( value )
  {
  }

  PrivacyItem::~PrivacyItem()
  {
  }

  bool PrivacyItem::operator==( PrivacyItem& item )
  {
    if( m_type == item.type()
        && m_action == item.action()
        && m_packetType == item.packetType()
        && m_value == item.value() )
      return true;
    else
      return false;
  }
};
