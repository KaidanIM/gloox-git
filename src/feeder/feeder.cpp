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

#include "../jlib/jclient.h"
#include "../jlib/roster.h"
#include "../jlib/disco.h"

using namespace std;


Feeder::Feeder( const string& id, const string& password, bool debug, int port )
  : m_poll( true ), m_infoHandler( 0 ), m_pollHandler( 0 ), m_debug( debug )
{
  c = new JClient( id, password, port );
  c->set_log_hook();
  c->setTls( false );
  c->setSasl( false );
  c->setDebug( debug );
  c->registerConnectionListener( this );
  c->roster()->registerRosterListener( this );
  c->registerIqHandler( this, XMLNS_IQ_RESULT );
  c->disco()->registerDiscoHandler( this );
  c->disco()->setVersion( "Feeder", "0.1" );
  c->disco()->addFeature( "xmppgrid:result" );
}

Feeder::~Feeder()
{
  delete c;
}

void Feeder::connect()
{
  c->connect();
}

bool Feeder::push( const char* data )
{
  RosterHelper::RosterMap roster = c->roster()->roster();
  RosterHelper::RosterMap::const_iterator it = roster.begin();
  for( it; it != roster.end(); it++ )
  {
    if( (*it).second == IKS_SHOW_AVAILABLE )
    {
      sendData( (*it).first );
      return true;
    }
  }
  m_poll = true;
  return false;
}

void Feeder::registerPollHandler( PollHandler* ph )
{
  m_pollHandler = ph;
}

void Feeder::itemAdded( const string& jid )
{
  if( m_infoHandler )
    m_infoHandler->itemAdded( jid );
}

void Feeder::itemRemoved( const string& jid )
{
  if( m_infoHandler )
    m_infoHandler->itemRemoved( jid );
}

void Feeder::itemAvailable( const string& jid, const char* msg )
{
  c->disco()->getDiscoInfo( jid );
}

void Feeder::itemChanged( const string& jid, int status, const char* msg )
{
  if( m_infoHandler )
    m_infoHandler->itemChanged( jid, status, msg );
}

bool Feeder::subscriptionRequest( const string& jid, const char* msg )
{
  if( m_infoHandler )
    return m_infoHandler->subscriptionRequest( jid, msg );
  return true;
}

void Feeder::roster( RosterHelper::RosterMap roster )
{
  if( m_infoHandler )
    m_infoHandler->roster( roster );
}

void Feeder::sendData( const string& jid )
{
  char* data = m_pollHandler->poll();
  if ( data )
  {
    iks* x = iks_make_iq( IKS_TYPE_SET, XMLNS_IQ_DATA );
    iks_insert_attrib( x, "from", c->jid().c_str() );
    iks_insert_attrib( x, "to", jid.c_str() );
    iks_insert_attrib( x, "id", "data" );
    iks* y = iks_first_tag( x );
    iks* z = iks_insert( y, "data" );
    iks_insert_cdata( z, data, iks_strlen( data ) );
    c->send( x );
    free( data );
  }
  else
    m_poll = false;

}

void Feeder::handleIq( const char* xmlns, ikspak* pak )
{
  if( iks_strncmp( XMLNS_IQ_RESULT, xmlns, iks_strlen( XMLNS_IQ_RESULT ) ) == 0 )
  {
    if( m_resultHandler )
    {
      char* data = iks_find_cdata( iks_child( pak->x ), "result" );
      if( data )
        m_resultHandler->handleResult( data );
    }

    iks* x = iks_make_iq( IKS_TYPE_RESULT, XMLNS_IQ_RESULT );
    iks_insert_attrib( x, "from", c->jid().c_str() );
    iks_insert_attrib( x, "to", pak->from->full );
    iks_insert_attrib( x, "id", pak->id );
    c->send( x );

    if( m_poll && m_pollHandler )
      sendData( pak->from->full );
  }
}

void Feeder::handleDiscoInfoResult( const string& id, const ikspak* pak )
{
  iks* y = iks_first_tag( iks_first_tag( pak->x ) );
  while( y )
  {
    if( ( iks_strcmp( iks_name( y ), "feature" ) == 0 )
          && ( iks_strncmp( XMLNS_IQ_DATA, iks_find_attrib( y, "var" ), iks_strlen( XMLNS_IQ_DATA ) ) == 0 ) )
    {
      printf( "%s supports xmppgrid:data\n", pak->from->full );
      if( m_poll && m_pollHandler )
        sendData( pak->from->full );
    }
    y = iks_next_tag( y );
  }
  iks_delete( y );
}

void Feeder::registerInfoHandler( InfoHandlerFeeder* ih )
{
  m_infoHandler = ih;
}

void Feeder::registerResultHandler( ResultHandler* rh )
{
  m_resultHandler = rh;
}

void Feeder::onConnect()
{
  if( m_infoHandler )
    m_infoHandler->connected();
}

void Feeder::onDisconnect()
{
  if( m_infoHandler )
    m_infoHandler->disconnected();
}
