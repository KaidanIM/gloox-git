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


#include "jclient.h"
#include "jthread.h"
#include "roster.h"
#include "disco.h"

#include <unistd.h>
#include <iostream>

#define JLIB_VERSION "0.1"


JClient::JClient()
  : m_port( XMPP_PORT ), m_thread( 0 ),
  m_tls( true ), m_sasl( true ),
  m_autoPresence( false ), m_manageRoster( true ),
  m_handleDisco( true ), m_idCount( 0 ), m_roster( 0 ),
  m_disco( 0 )
{
  init();
}

JClient::JClient( const std::string& id, const std::string& password, int port )
  : m_port( port ), m_password( password ), m_thread( 0 ),
  m_tls( true ), m_sasl( true ),
  m_autoPresence( false ), m_manageRoster( true ),
  m_handleDisco( true ), m_idCount( 0 ), m_roster( 0 ),
  m_disco( 0 )
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
  m_tls( true ), m_sasl( true ),
  m_autoPresence( false ), m_manageRoster( true ),
  m_handleDisco( true ), m_idCount( 0 ), m_roster( 0 ),
  m_disco( 0 )
{
  init();
}

JClient::~JClient()
{
}

void JClient::init()
{
  m_disco = new Disco( this );
  m_roster = new Roster( this );
  m_disco->setVersion( "JLib", JLIB_VERSION );
  m_disco->setIdentity( "client", "bot" );
}

void JClient::cleanUp()
{
  iks_filter_delete( m_filter );
  delete m_disco;
  delete m_roster;
  delete m_thread;
}

void JClient::on_stream( int type, iks* node )
{
  if( m_debug ) printf("in on_stream\n");
  ikspak* pak = iks_packet( node );

  switch (type)
  {
    case IKS_NODE_START:      // <stream:stream>
      if ( m_tls && !is_secure() )
      {
        start_tls();
        if( m_debug ) printf("after starttls\n");
        break;
      }
      if ( !m_sasl )
        login( iks_find_attrib ( node, "id" ) );
      break;
      case IKS_NODE_NORMAL:     // first level child of stream
        if ( strncmp( "stream:features", iks_name( node ), 15 ) == 0 ) {
          m_streamFeatures = iks_stream_features( node );
          if ( m_sasl ) {
            if ( m_tls && !is_secure() )
              break;
            if ( m_authorized ) {
              iks* t;
              if ( m_streamFeatures & IKS_STREAM_BIND ) {
                send( iks_make_resource_bind( m_self ) );
              }
              if ( m_streamFeatures & IKS_STREAM_SESSION ) {
                iks* x = iks_make_session();
                iks_insert_attrib( x, "id", "auth" );
                send( x );
              }
            } else {
              if ( m_streamFeatures & IKS_STREAM_SASL_MD5 )
                start_sasl( IKS_SASL_DIGEST_MD5, (char *) username().c_str(), (char *) password().c_str() );
              else if ( m_streamFeatures & IKS_STREAM_SASL_PLAIN )
                start_sasl( IKS_SASL_PLAIN, (char *) username().c_str(), (char *) password().c_str() );
            }
          }
        } else if ( strcmp ( "failure", iks_name ( node ) ) == 0 ) {
          if( m_debug ) printf("sasl authentication failed...\n");
          m_state = STATE_AUTHENTICATION_FAILED;
        } else if ( strcmp ( "success", iks_name ( node ) ) == 0 ) {
          if( m_debug ) printf( "sasl initialisation successful...\n" );
          m_state = STATE_AUTHENTICATED;
          m_authorized = true;
          header( server() );
        } else {
          ikspak* pak;
          pak = iks_packet ( node );
          iks_filter_packet ( m_filter, pak );
        }
        break;
    case IKS_NODE_ERROR:      // <stream:error>
//       syslog(LOG_ERR, "stream error. quitting\n");
//       logout();
      break;
    case IKS_NODE_STOP:       // </stream:stream>
      break;
  }
//   if( m_debug ) printf("at the end of on_stream\n");
  iks_delete( node );
}


void JClient::on_log( const char* data, size_t size, int is_incoming ) {
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
  char* tmp = (char*)malloc( strlen( "id" ) + sizeof( int ) );
  sprintf( tmp, "uid%d", ++m_idCount );
  std::string str( tmp );
  free( tmp );
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
  iks_filter_add_rule( m_filter, (iksFilterHook*) errorHook, this,
                      IKS_RULE_TYPE, IKS_PAK_IQ,
                      IKS_RULE_SUBTYPE, IKS_TYPE_ERROR,
                      IKS_RULE_DONE );
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

void JClient::connect()
{
  if( jid().empty() )
    return;

  m_self = iks_id_new( get_stack(), jid().c_str() );
  setupFilter();

  m_state = STATE_CONNECTING;
  int ret;
  if(ret = Stream::connect( m_server, m_port ) )
  {
    switch( ret )
    {
      case IKS_NET_NODNS:
        printf( "host name lookup failure: %s\n", m_server.c_str() );
        break;
      case IKS_NET_NOSOCK:
        printf( "cannot create socket\n" );
        break;
      case IKS_NET_NOCONN:
        printf( "connection refused or no xml stream: %s:%d\n", m_server.c_str(), m_port );
        break;
      case IKS_NET_RWERR:
        printf( "read/write error: %s\n", m_server.c_str() );
        break;
    }
    return;
  }

  m_state = STATE_CONNECTED;

  m_thread = new JThread( this );
  m_thread->start();

  while( m_state >= STATE_CONNECTED &&
         m_state != STATE_AUTHENTICATED &&
         m_state != STATE_AUTHENTICATION_FAILED )
  {
    JThread::sleep( 1000 );
  }

  if ( m_state == STATE_AUTHENTICATION_FAILED )
  {
    if( m_createAccount )
    {
//       createAccount();
    }
  }

  m_thread->join();

  cleanUp();
}

void JClient::disconnect()
{
  if( m_state != STATE_DISCONNECTED )
  {
    m_thread->cancel();
    sleep(1);
    Stream::disconnect();
    m_state = STATE_DISCONNECTED;
  }
}

void JClient::send( iks* x )
{
  Stream::send( this->P, x );
  iks_delete( x );
}

void JClient::sendPresence()
{
  iks* x = iks_make_pres( IKS_SHOW_AVAILABLE, "online" );
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

void JClient::registerIqHandler( IqHandler* ih )
{
  m_iqHandlers.push_back( ih );
}

void JClient::removeIqHandler( const char* xmlns )
{
  m_iqNSHandlers.erase( xmlns );
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
  for( it; it != m_connectionListeners.end(); it++ ) {
    (*it)->onConnect();
  }
}

void JClient::notifyOnDisconnect()
{
  ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
  for( it; it != m_connectionListeners.end(); it++ ) {
    (*it)->onDisconnect();
  }
}

void JClient::notifyPresenceHandlers( iksid* from, iksubtype type, ikshowtype show, const char* msg )
{
  PresenceHandlerList::const_iterator it = m_presenceHandlers.begin();
  for( it; it != m_presenceHandlers.end(); it++ ) {
    (*it)->handlePresence( from, type, show, msg );
  }
}

void JClient::notifySubscriptionHandlers( iksid* from, iksubtype type, const char* msg )
{
  SubscriptionHandlerList::const_iterator it = m_subscriptionHandlers.begin();
  for( it; it != m_subscriptionHandlers.end(); it++ ) {
    (*it)->handleSubscription( from, type, msg );
  }
}

void JClient::notifyIqHandlers( const char* xmlns, ikspak* pak )
{
  IqHandlerList::const_iterator it = m_iqHandlers.begin();
  for( it; it != m_iqHandlers.end(); it++ ) {
    (*it)->handleIq( xmlns, pak );
  }

  IqHandlerMap::const_iterator it_ns = m_iqNSHandlers.begin();
  for( it_ns; it_ns != m_iqNSHandlers.end(); it_ns++ ) {
    if( iks_strncmp( (*it_ns).first, xmlns, iks_strlen( xmlns ) ) == 0 )
      (*it_ns).second->handleIq( xmlns, pak );
  }
}

void JClient::notifyMessageHandlers( iksid* from, iksubtype type, const char* msg )
{
  MessageHandlerList::const_iterator it = m_messageHandlers.begin();
  for( it; it != m_messageHandlers.end(); it++ ) {
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
//   if (config.autoreg)
//   {
//     iks *x, *y;
//     iksid* id;
//     id = iks_id_new(m_stack, config.jabberID);
// 
//     x = iks_new("iq");
//     iks_insert_attrib(x, "type", "set");
//     iks_insert_attrib(x, "id", "reg");
//     y = iks_insert(x, "query");
//     iks_insert_attrib(y, "xmlns", XMLNS_REGISTER );
//     iks_insert_cdata(iks_insert(y, "username"), id->user, strlen(id->user));
//     iks_insert_cdata(iks_insert(y, "password"), config.jabberPwd, strlen(config.jabberPwd));
//     iks_send(m_prs, x);
// //    iks_delete(y);
//     iks_delete(x);
//   }
//   else
//   {
//     if( stream->debug() ) printf("jabber account does not exist and autoreg is false. exiting.\n");
//     m_continue = 0;
//   }
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
