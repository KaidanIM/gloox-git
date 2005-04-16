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
  m_parent->registerIqHandler( this, XMLNS_ROSTER );
}

Roster::~Roster()
{

}

Roster::RosterMap Roster::listRoster()
{
  return m_roster;
}

void Roster::fill()
{
  iks* x = iks_make_iq( IKS_TYPE_GET, XMLNS_ROSTER );
  iks_insert_attrib( x, "id", "roster_get" );
  m_parent->send( x );
}

void Roster::handleIq( const char* xmlns, ikspak* pak )
{
  if( pak->subtype == IKS_TYPE_RESULT )
  {
    if( iks_strncmp( xmlns, XMLNS_ROSTER, strlen( XMLNS_ROSTER ) ) == 0 )
    {
      printf( "roster arriving\n");
      iks* y = iks_first_tag( iks_first_tag( pak->x ) );
      while( y )
      {
        if( strcmp( iks_name( y ), "item" ) == 0 )
        {
          char* jid = iks_find_attrib( y, "jid" );
          add( jid, IKS_TYPE_UNAVAILABLE );
          printf( "received roster item: %s\n", jid );
        }
        y = iks_next_tag( y );
      }
    }
  }
}

void Roster::add( const string& jid)
{
  
}

void Roster::add( const string& jid, int status)
{
  m_roster[jid] = status;
}
