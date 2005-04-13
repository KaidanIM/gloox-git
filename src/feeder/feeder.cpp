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



#include "feeder.h"

using namespace std;


Feeder::Feeder( const string username, const string resource,
                const string password, const string server,
                int port, bool debug )
  : m_poll( true )
{
  c = new JClient( username, resource, password, server, port );
  c->set_log_hook();
  c->setTls( false );
  c->setSasl( false );
  c->setDebug( debug );
  c->registerConnectionListener( this );
//   c->registerIqHandler( this );
  c->setVersion( "Feeder", "0.1" );
  c->connect();
}

Feeder::~Feeder()
{
  
}

bool Feeder::push( const char* data )
{
  PresenceList::const_iterator it = m_presence.begin();
  for( it; it != m_presence.end(); ++it )
  {
    if( (*it).second == "available" )
    {
      c->send( (*it).first, data );
      return true;
    }
  }
  return false;
}

void Feeder::registerPollHandler( PollHandler* ph )
{
  m_pollHandler = ph;
}

void Feeder::handlePresence( iksid* from, iksubtype type, ikshowtype show, const char* msg )
{
  m_presence[from->full] = (char*) msg;
  if ( ( strncmp( msg, "available", 9 ) == 0 ) && m_poll )
  {
    char* data = m_pollHandler->poll();
    if ( data )
      c->send( from->full, data );
  }
}

void Feeder::handleIq( const char* xmlns, ikspak* pak )
{
  
}

void Feeder::onConnect()
{
  
}

void Feeder::onDisconnect()
{
  
}
