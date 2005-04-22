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
#include "../jlib/disco.h"

using namespace std;


Worker::Worker( const string& id, const string& password, const bool debug, const int port )
  : m_infoHandler( 0 ), m_dataHandler( 0 ), m_working( false ),
  m_feederID( 0 ), m_debug( debug )
{
  c = new JClient( id, password, port );
  c->set_log_hook();
  c->setTls( false );
  c->setSasl( false );
  c->setDebug( debug );
  c->registerIqHandler( this, XMLNS_IQ_DATA );
  c->registerConnectionListener( this );
  c->roster()->registerRosterListener( this );
  c->disco()->setVersion( "Worker", "0.1" );
  c->disco()->addFeature( "xmppgrid:data" );
}

Worker::~Worker()
{
}

void Worker::setFeeder( const string& jid )
{
  m_feederID = iks_id_new( c->get_stack(), jid.c_str() );
}

void Worker::connect()
{
  c->connect();
}

void Worker::disconnect()
{
  c->disconnect();
  delete c;
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
      iks_insert_attrib( x, "id", pak->id );
      c->send( x );
      if( m_debug ) printf( "got packet but also got work\n");
    }
    else
    {
      m_working = true;
      if( m_debug ) printf( "got packet, now working\n" );
      iks* x = iks_make_iq( IKS_TYPE_RESULT, pak->ns );
      iks_insert_attrib( x, "from", c->jid().c_str() );
      iks_insert_attrib( x, "to", pak->from->full );
      iks_insert_attrib( x, "id", pak->id );
      c->send( x );

      if( m_dataHandler )
      {
        char* t = iks_find_cdata( iks_find( pak->x, "query" ), "data" );
        m_dataHandler->data( t );
      }
    }
  }
  else
    if( m_debug ) printf( "unhandled xmlns: %s\n", xmlns );
}

void Worker::registerDataHandler( DataHandler* dh )
{
  m_dataHandler = dh;
}

void Worker::registerInfoHandler( InfoHandlerWorker* ih )
{
  m_infoHandler = ih;
}

void Worker::result( ResultCode code, char* result )
{
  m_working = false;
  if( m_debug ) printf( "work finished: %s\n", result );
  iks* x = iks_make_iq( IKS_TYPE_SET, XMLNS_IQ_RESULT );
  iks_insert_attrib( x, "from", c->jid().c_str() );
  iks_insert_attrib( x, "to", m_feederID->full );
  iks_insert_attrib( x, "id", "result" );
  iks* y = iks_first_tag( x );
  iks* z = iks_insert( y, "result" );
  char* r = (char*)malloc( sizeof( int ) );;
  sprintf(r, "%d", code );
  iks_insert_attrib( z, "result", r );
  iks_insert_cdata( z, result, iks_strlen( result ) );
  c->send( x );
  free( r );
  free( result );
}

bool Worker::subscriptionRequest( const string& jid, const char* msg )
{
  if( jid == m_feederID->partial )
    return true;

  return false;
}

void Worker::onConnect()
{
  c->roster()->subscribe( m_feederID->partial );

  if( m_infoHandler )
    m_infoHandler->connected();
}

void Worker::onDisconnect()
{
  if( m_infoHandler )
    m_infoHandler->disconnected();
}
