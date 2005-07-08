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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/


#include "jclient.h"
#include "jthread.h"
#include "roster.h"
#include "disco.h"
#include "adhoc.h"
#include "prep.h"

#include <unistd.h>
#include <iostream>

#define GLOOX_VERSION "0.2"


JClient::JClient()
  : m_port( XMPP_PORT ), m_thread( 0 ),
  m_tls( true ), m_sasl( true ), m_priority( -1 ),
  m_autoPresence( false ), m_manageRoster( true ),
  m_handleDisco( true ), m_idCount( 0 ), m_roster( 0 ),
  m_disco( 0 ), m_adhoc( 0 )
{
  init();
}

JClient::JClient( const std::string& id, const std::string& password, int port )
  : m_port( port ), m_password( password ), m_thread( 0 ),
  m_tls( true ), m_sasl( true ), m_priority( -1 ),
  m_autoPresence( false ), m_manageRoster( true ),
  m_handleDisco( true ), m_idCount( 0 ), m_roster( 0 ),
  m_disco( 0 ), m_adhoc( 0 )
{
  m_self = iks_id_new( get_stack(), id.c_str() );
  m_username = m_self->user;
  m_server = m_self->server;
  m_resource = m_self->resource;
  init();
}

JClient::JClient( const std::string& username, const std::string& password, const std::string& server,
                  const std::string& resource, int port )
  : m_username( username ), m_resource( resource ), m_password( password ),
  m_server( server ), m_port( port ), m_thread( 0 ),
  m_tls( true ), m_sasl( true ), m_priority( -1 ),
  m_autoPresence( false ), m_manageRoster( true ),
  m_handleDisco( true ), m_idCount( 0 ), m_roster( 0 ),
  m_disco( 0 ), m_adhoc( 0 )
{
  init();
}

JClient::~JClient()
{
}

void JClient::init()
{
  m_disco = new Disco( this );
  m_adhoc = new Adhoc( this );
  m_roster = new Roster( this );
  m_disco->setVersion( "based on gloox", GLOOX_VERSION );
  m_disco->setIdentity( "client", "bot" );
}

void JClient::cleanUp()
{
  iks_filter_delete( m_filter );
  delete m_disco;
  delete m_adhoc;
  delete m_roster;
  delete m_thread;
}

std::string JClient::jid()
{
  if( server().empty() )
    return "";
  else if( username().empty() )
    if( resource().empty() )
      return server();
    else
      return ( server() + "/" + resource() );
  else
    if( resource().empty() )
      return ( username() + "@" + server() );
    else
      return ( username() + "@" + server() + "/" + resource() );
}

void JClient::on_stream( int type, iks* node )
{
  if(!node)
    return;

  if( m_debug ) printf("in on_stream\n");
  ikspak* pak = iks_packet( node );

  switch (type)
  {
    case IKS_NODE_START:      // <stream:stream>
      break;
    case IKS_NODE_NORMAL:     // first level child of stream
      if ( strncmp( "stream:features", iks_name( node ), 15 ) == 0 )
      {
        m_streamFeatures = iks_stream_features( node );

        if ( m_tls && !is_secure() && ( m_streamFeatures & IKS_STREAM_STARTTLS ) )
        {
          start_tls();
          if( m_debug ) printf("after starttls\n");
          break;
        }

        if ( m_sasl )
        {
          if( m_tls  && !is_secure() )
            break;

          if ( m_authorized )
          {
            if ( m_streamFeatures & IKS_STREAM_BIND )
            {
              send( make_resource_bind( m_self ) );
            }

            if ( m_streamFeatures & IKS_STREAM_SESSION )
            {
              iks* x = iks_make_session();
              iks_insert_attrib( x, "id", "auth" );
              send( x );
            }
          }
          else if( !username().empty() || !password().empty() )
          {
            if ( m_streamFeatures & IKS_STREAM_SASL_MD5 )
              start_sasl( IKS_SASL_DIGEST_MD5, (char *) username().c_str(), (char *) password().c_str() );
            else if ( m_streamFeatures & IKS_STREAM_SASL_PLAIN )
              start_sasl( IKS_SASL_PLAIN, (char *) username().c_str(), (char *) password().c_str() );
          }
          else
          {
            notifyOnConnect();
          }
        }
      }
      else if ( iks_strncmp( "failure", iks_name ( node ), 7 ) == 0 )
      {
        if( m_debug ) printf("sasl authentication failed...\n");
        m_state = STATE_AUTHENTICATION_FAILED;
      }
      else if ( iks_strncmp( "success", iks_name ( node ), 7 ) == 0 )
      {
        if( m_debug ) printf( "sasl initialisation successful...\n" );
        m_state = STATE_AUTHENTICATED;
        m_authorized = true;
        header( server() );
      }
      else
      {
        ikspak* pak;
        pak = iks_packet ( node );
        iks_filter_packet ( m_filter, pak );
      }
      break;
    case IKS_NODE_ERROR:      // <stream:error>
      m_state = STATE_ERROR;
      if( m_debug ) printf( "stream error. quitting\n");
      disconnect();
      break;
    case IKS_NODE_STOP:       // </stream:stream>
      disconnect();
      break;
  }

  iks_delete( node );
}


void JClient::on_log( const char* data, size_t size, int is_incoming )
{
  if( m_debug )
  {
    if ( is_secure() )
      cerr << "Sec";

    if (is_incoming)
      cerr << "RECV ";
    else
      cerr << "SEND ";

    cerr << "[" << data << "]" << endl;
  }
}

void JClient::disableDisco()
{
  m_handleDisco = false;
  delete m_roster;
  m_roster = 0;
}

void JClient::disableRoster()
{
  m_manageRoster = false;
  delete m_roster;
  m_roster = 0;
}

std::string JClient::getID()
{
  char tmp[10];
  sprintf( tmp, "uid%d", ++m_idCount );
  std::string str( tmp );
  return str;
}

void JClient::login( const char* sid )
{
  if( m_debug ) printf("in login()\n");

  iks* x = iks_make_auth( m_self, password().c_str(), sid );
  iks_insert_attrib( x, "id", "auth" );
  send( x );
}

void JClient::setupFilter()
{
  m_filter = iks_filter_new();

  iks_filter_add_rule( m_filter, (iksFilterHook*) authHook, this,
                      IKS_RULE_TYPE, IKS_PAK_IQ,
                      IKS_RULE_SUBTYPE, IKS_TYPE_RESULT,
                      IKS_RULE_ID, "auth",
                      IKS_RULE_DONE );
  iks_filter_add_rule( m_filter, (iksFilterHook*) registerHook, this,
                      IKS_RULE_TYPE, IKS_PAK_IQ,
                      IKS_RULE_SUBTYPE, IKS_TYPE_ERROR,
                      IKS_RULE_ID, "auth",
                      IKS_RULE_DONE );
  iks_filter_add_rule( m_filter, (iksFilterHook*) registeredHook, this,
                      IKS_RULE_TYPE, IKS_PAK_IQ,
                      IKS_RULE_SUBTYPE, IKS_TYPE_RESULT,
                      IKS_RULE_ID, "reg",
                      IKS_RULE_DONE );
//   iks_filter_add_rule( m_filter, (iksFilterHook*) errorHook, this,
//                       IKS_RULE_TYPE, IKS_PAK_IQ,
//                       IKS_RULE_SUBTYPE, IKS_TYPE_ERROR,
//                       IKS_RULE_DONE );
  iks_filter_add_rule( m_filter, (iksFilterHook*) msgHook, this,
                      IKS_RULE_TYPE, IKS_PAK_MESSAGE,
                      IKS_RULE_DONE );
  iks_filter_add_rule( m_filter, (iksFilterHook*) presenceHook, this,
                      IKS_RULE_TYPE, IKS_PAK_PRESENCE,
                      IKS_RULE_DONE );
  iks_filter_add_rule( m_filter, (iksFilterHook*) subscriptionHook, this,
                      IKS_RULE_TYPE, IKS_PAK_S10N,
                      IKS_RULE_DONE );
  iks_filter_add_rule( m_filter, (iksFilterHook*) iqHook, this,
                      IKS_RULE_TYPE, IKS_PAK_IQ,
                      IKS_RULE_DONE );
}

void JClient::connect( bool blocking )
{
  if( server().empty() )
    return;

  m_blockingConnect = blocking;

  m_self = iks_id_new( get_stack(), jid().c_str() );
  setupFilter();

  m_state = STATE_CONNECTING;
  int ret;
  if(ret = Stream::connect( Prep::idna( m_server ), m_port, server() ) )
  {
    switch( ret )
    {
      case IKS_NET_NODNS:
        printf( "host name lookup failure: %s\n", Prep::idna( m_server.c_str() ).c_str() );
        break;
      case IKS_NET_NOSOCK:
        printf( "cannot create socket\n" );
        break;
      case IKS_NET_NOCONN:
        printf( "connection refused or no xml stream: %s:%d\n", Prep::idna( m_server.c_str() ).c_str(), m_port );
        break;
      case IKS_NET_RWERR:
        printf( "read/write error: %s\n", Prep::idna( m_server.c_str() ).c_str() );
        break;
    }
    return;
  }

  m_state = STATE_CONNECTED;

  m_thread = new JThread( this );
  m_thread->start();

  if( !username().empty() && !password().empty() )
  {
    while( m_state >= STATE_CONNECTED &&
           m_state != STATE_AUTHENTICATED &&
           m_state != STATE_AUTHENTICATION_FAILED )
    {
      JThread::sleep( 1000 );
    }

    if ( m_state == STATE_AUTHENTICATION_FAILED )
    {
      notifyOnDisconnect();
    }
  }

  if( m_blockingConnect )
  {
    m_thread->join();
    cleanUp();
  }
}

void JClient::disconnect()
{
  if( m_state != STATE_DISCONNECTED )
  {
    m_state = STATE_DISCONNECTED;
    m_thread->cancel();
    Stream::disconnect();

    if( !m_blockingConnect )
    {
      m_thread->join();
      cleanUp();
    }
  }
}

void JClient::send( iks* x )
{
  Stream::send( this->P, x );
  iks_delete( x );
}

void JClient::sendPresence()
{
  char prio[5];
  sprintf( prio, "%d", m_priority );
  iks* x = iks_make_pres( IKS_SHOW_AVAILABLE, "online" );
  iks_insert_cdata( iks_insert( x, "priority" ), prio, iks_strlen( prio ) );
  send( x );
}

void JClient::send( const char* jid, const char* data )
{
  iks* x = iks_make_msg( IKS_TYPE_NONE, jid, data );
  send( x );
}

JClient::StateEnum JClient::clientState()
{
  return m_state;
}

void JClient::setClientState( StateEnum s )
{
  m_state = s;
}

iksparser* JClient::parser()
{
  return this->P;
}

Roster* JClient::roster()
{
  return m_roster;
}

Disco* JClient::disco()
{
  return m_disco;
}

Adhoc* JClient::adhoc()
{
  return m_adhoc;
}

void JClient::registerPresenceHandler( PresenceHandler* ph )
{
  m_presenceHandlers.push_back( ph );
}

void JClient::removePresenceHandler( PresenceHandler* ph )
{
  m_presenceHandlers.remove( ph );
}

void JClient::registerIqHandler( IqHandler* ih, const char* xmlns )
{
  m_iqNSHandlers[xmlns] = ih;
}

void JClient::registerIqFTHandler( IqHandler* ih, const char* tag )
{
  m_iqFTHandlers[tag] = ih;
}

void JClient::trackID( IqHandler* ih, const char* id )
{
  m_iqIDHandlers[id] = ih;
}

void JClient::registerIqHandler( IqHandler* ih )
{
  m_iqHandlers.push_back( ih );
}

void JClient::removeIqNSHandler( const char* xmlns )
{
  m_iqNSHandlers.erase( xmlns );
}

void JClient::removeIqFTHandler( const char* tag )
{
  m_iqFTHandlers.erase( tag );
}

void JClient::removeIqHandler( IqHandler* ih )
{
  m_iqHandlers.remove( ih );
}

void JClient::registerMessageHandler( MessageHandler* mh )
{
  m_messageHandlers.push_back( mh );
}

void JClient::removeMessageHandler( MessageHandler* mh )
{
  m_messageHandlers.remove( mh );
}

void JClient::registerSubscriptionHandler( SubscriptionHandler* sh )
{
  m_subscriptionHandlers.push_back( sh );
}

void JClient::removeSubscriptionHandler( SubscriptionHandler* sh )
{
  m_subscriptionHandlers.remove( sh );
}

void JClient::registerConnectionListener( ConnectionListener* cl )
{
  m_connectionListeners.push_back( cl );
}

void JClient::removeConnectionListener( ConnectionListener* cl )
{
  m_connectionListeners.remove( cl );
}

void JClient::notifyOnConnect()
{
  if( m_manageRoster )
    m_roster->fill();

  if( m_autoPresence )
    sendPresence();

  ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
  for( it; it != m_connectionListeners.end(); it++ )
  {
    (*it)->onConnect();
  }
}

void JClient::notifyOnDisconnect()
{
  ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
  for( it; it != m_connectionListeners.end(); it++ )
  {
    (*it)->onDisconnect();
  }
}

void JClient::notifyPresenceHandlers( iksid* from, iksubtype type, ikshowtype show, const char* msg )
{
  PresenceHandlerList::const_iterator it = m_presenceHandlers.begin();
  for( it; it != m_presenceHandlers.end(); it++ )
  {
    (*it)->handlePresence( from, type, show, msg );
  }
}

void JClient::notifySubscriptionHandlers( iksid* from, iksubtype type, const char* msg )
{
  SubscriptionHandlerList::const_iterator it = m_subscriptionHandlers.begin();
  for( it; it != m_subscriptionHandlers.end(); it++ )
  {
    (*it)->handleSubscription( from, type, msg );
  }
}

void JClient::notifyIqHandlers( const char* xmlns, ikspak* pak )
{
  IqHandlerList::const_iterator it = m_iqHandlers.begin();
  for( it; it != m_iqHandlers.end(); it++ )
  {
    (*it)->handleIq( xmlns, pak );
  }

  IqHandlerMap::const_iterator it_ns = m_iqNSHandlers.begin();
  for( it_ns; it_ns != m_iqNSHandlers.end(); it_ns++ )
  {
    if( iks_strncmp( (*it_ns).first, xmlns, iks_strlen( xmlns ) ) == 0 )
      (*it_ns).second->handleIq( xmlns, pak );
  }

  IqHandlerMap::const_iterator it_id = m_iqIDHandlers.begin();
  for( it_id; it_id != m_iqIDHandlers.end(); it_id++ )
  {
#warning FIXME: do not use the length of pak->id here!
    if( iks_strncmp( (*it_id).first, pak->id, iks_strlen( pak->id/*(*it_id).first*/ ) ) == 0 )
    {
      (*it_id).second->handleIqID( pak->id, pak );
      m_iqIDHandlers.erase( pak->id );
    }
  }

  char* tag = iks_name( iks_first_tag( pak->x ) );
  IqHandlerMap::const_iterator it_ft = m_iqFTHandlers.begin();
  for( it_ft; it_ft != m_iqFTHandlers.end(); it_ft++ )
  {
    if( iks_strncmp( (*it_ft).first, tag, iks_strlen( tag ) ) == 0 )
      (*it_ft).second->handleIqTag( tag, pak );
  }
}

void JClient::notifyMessageHandlers( iksid* from, iksubtype type, const char* msg )
{
  MessageHandlerList::const_iterator it = m_messageHandlers.begin();
  for( it; it != m_messageHandlers.end(); it++ )
  {
    (*it)->handleMessage( from, type, msg );
  }
}

int authHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("authHook\n");
  stream->notifyOnConnect();
  return IKS_FILTER_EAT;
}

int registerHook( JClient* stream, ikspak* pak )
{
  stream->setClientState( JClient::STATE_AUTHENTICATION_FAILED );
  return IKS_FILTER_EAT;
}

int registeredHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("registeredHook\n");
  stream->setClientState( JClient::STATE_AUTHENTICATED );
  return IKS_FILTER_EAT;
}

int msgHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("msgHook\n");
  stream->notifyMessageHandlers( pak->from, pak->subtype, iks_find_cdata( pak->x, "body" ) );
  return IKS_FILTER_EAT;
}

int iqHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("iqHook\n");
  stream->notifyIqHandlers( pak->ns, pak );
  return IKS_FILTER_EAT;
}

int presenceHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("presenceHook\n");
  stream->notifyPresenceHandlers( pak->from, pak->subtype, pak->show, iks_find_cdata( pak->x, "status" ) );
  return IKS_FILTER_EAT;
}

int subscriptionHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("subscriptionHook\n");
  stream->notifySubscriptionHandlers( pak->from, pak->subtype, iks_find_cdata( pak->x, "status" ) );
  return IKS_FILTER_EAT;
}

int errorHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("errorHook\n");
  return IKS_FILTER_EAT;
}
