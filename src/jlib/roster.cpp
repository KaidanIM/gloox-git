/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>

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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/


#include "roster.h"


Roster::Roster( JClient* parent )
  : m_parent( parent )
{
  m_parent->registerIqHandler( this, "jabber:iq:roster" );
}

Roster::~Roster()
{

}

Roster::RosterMap /*map<const string, string>*/ Roster::listRoster()
{
  return m_roster;
}

void Roster::fill()
{
  iks* x = iks_new( "iq" );
  iks_insert_attrib( x, "from", m_parent->jid().c_str() );
  iks_insert_attrib( x, "type", "get" );
  iks_insert_attrib( x, "id", "roster_get" );
  iks* y = iks_insert( x, "query" );
  iks_insert_attrib( y, "xmlns", "jabber:iq:roster" );
  m_parent->send( x );
}

void Roster::handleIq( const char* xmlns, ikspak* oak )
{
  
}
