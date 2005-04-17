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




#include "worker.h"

using namespace std;


Worker::Worker( const string username, const string resource,
                const string password, const string server,
                int port, bool debug )
  : m_infoHandler( 0 ), m_dataHandler( 0 )
{
  c = new JClient( username, resource, password, server, port );
  c->set_log_hook();
  c->setTls( false );
  c->setSasl( false );
  c->setDebug( debug );
  c->registerIqHandler( this, XMLNS_IQ_DATA );
  c->registerIqHandler( this, XMLNS_IQ_RESULT );
  c->roster()->registerRosterListener( this );
  c->setVersion( "Worker", "0.1" );

  m_feederJID = strdup( "jline@camaya.net/feeder" );
}

Worker::~Worker()
{
  
}

void Worker::connect()
{
  c->connect();
}

void Worker::handleIq( const char* xmlns, ikspak* pak )
{
  if( iks_strncmp( XMLNS_IQ_DATA, xmlns, iks_strlen( XMLNS_IQ_DATA ) ) == 0 )
  {
    if( m_working )
      printf( "got packet but also got work\n");
    else
      printf( "got packet, now working\n" );
  }
  else if( iks_strncmp( XMLNS_IQ_RESULT, xmlns, iks_strlen( XMLNS_IQ_RESULT ) ) == 0 )
  {
    printf( "got result but should not get one. came from %s\n", pak->from->full);
  }
  else
    printf( "unhandled xmlns: %s\n", xmlns );
}

void Worker::handleSubscription( iksid* from, iksubtype type, const char* msg )
{
  switch( type )
  {
    case IKS_TYPE_SUBSCRIBE:
      if( iks_strncmp( m_feederJID, from->full, iks_strlen( m_feederJID ) ) == 0 )
      {
        iks* x = iks_make_s10n( IKS_TYPE_SUBSCRIBED, from->full, "ok" );
        iks_insert_attrib( x, "id", "sub01" );
        c->send( x );
      }
      break;
    case IKS_TYPE_SUBSCRIBED:
      printf( "new buddy: %s\n", from->full );
      break;
    case IKS_TYPE_UNSUBSCRIBE:
      {
        iks* y = iks_make_s10n( IKS_TYPE_SUBSCRIBED, from->full, "ok" );
        iks_insert_attrib( y, "id", "sub02" );
        c->send( y );
        break;
      }
    case IKS_TYPE_UNSUBSCRIBED:
      printf( "buddy removed: %s\n", from->full );
      break;
  }
}

void Worker::registerDataHandler( DataHandler* dh )
{
  m_dataHandler = dh;
}

void Worker::registerInfoHandler( InfoHandlerWorker* ih )
{
  m_infoHandler = ih;
}

void Worker::result( ResultCode code, const char* result )
{
  
}
