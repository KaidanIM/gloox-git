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

#include "../common/common.h"
#include "../jlib/jclient.h"
#include "../jlib/roster.h"

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
  c->registerConnectionListener( this );
  c->roster()->registerRosterListener( this );
  c->setVersion( "Worker", "0.1" );

  m_feederJID = strdup( "remon@camaya.net" );
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
    {
      iks* x = iks_make_iq( IKS_TYPE_ERROR, XMLNS_IQ_DATA );
      iks_insert_attrib( x, "from", c->jid().c_str() );
      iks_insert_attrib( x, "to", pak->from->full );
      c->send( x );
      printf( "got packet but also got work\n");
    }
    else
    {
      printf( "got packet, now working\n" );
      iks* x = iks_make_pres( IKS_SHOW_AWAY, "busy" );
      c->send( x );
      if( m_dataHandler )
        m_dataHandler->data( "packet" );
      m_working = true;
    }
  }
  else
    printf( "unhandled xmlns: %s\n", xmlns );
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
  m_working = false;
  iks* x = iks_make_iq( IKS_TYPE_RESULT, XMLNS_IQ_RESULT );
  iks_insert_attrib( x, "from", c->jid().c_str() );
  iks_insert_attrib( x, "to", m_feederJID.c_str() );
  c->send( x );
}

bool Worker::subscriptionRequest( const string& jid, const char* msg )
{
  if( jid == m_feederJID )
    return true;

  return false;
}

void Worker::onConnect()
{
  c->roster()->subscribe( m_feederJID );

  if( m_infoHandler )
    m_infoHandler->connected();
}

void Worker::onDisconnect()
{
  if( m_infoHandler )
    m_infoHandler->disconnected();
}
