/*
 * (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
 */


#include "jclient.h"
#include "jthread.h"

#include <iostream>

// using namespace Iksemel;


JClient::JClient()
  : m_username( 0 ), m_resource( 0 ), m_password( 0 ),
  m_server( 0 ), m_port( 0 ), m_thread( 0 ),
  m_tls( true ), m_sasl( true ),
  m_autoPresence( true )
{
  init();
}

JClient::JClient( const std::string username, const std::string resource, const std::string password,
                  const std::string server, int port )
  : m_username( username ), m_resource( resource ), m_password( password ),
  m_server( server ), m_port( port ), m_thread( 0 ),
  m_tls( true ), m_sasl( true ),
  m_autoPresence( true )
{
  init();
}

JClient::~JClient()
{
}

void JClient::init()
{
  no_version( false );
}

void JClient::on_stream( int type, iks* node )
{
  if( m_debug ) printf("in on_stream\n");
  ikspak* pak = iks_packet( node );
//   iks_filter_packet(m_filter, pak);
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
          m_features = iks_stream_features( node );
          if ( m_sasl ) {
            if ( m_tls && !is_secure() )
              break;
            if ( m_authorized ) {
              iks* t;
              if ( m_features & IKS_STREAM_BIND ) {
                send( iks_make_resource_bind( m_self ) );
              }
              if ( m_features & IKS_STREAM_SESSION ) {
                iks* x = iks_make_session();
                iks_insert_attrib( x, "id", "auth" );
                send( x );
              }
            } else {
              if ( m_features & IKS_STREAM_SASL_MD5 )
                start_sasl( IKS_SASL_DIGEST_MD5, (char *) username().c_str(),
                            (char *) password().c_str() );
              else if ( m_features & IKS_STREAM_SASL_PLAIN )
                start_sasl( IKS_SASL_PLAIN, (char *) username().c_str(),
                            (char *) password().c_str() );
            }
          }
//           break;
        } else if ( strcmp ( "failure", iks_name ( node ) ) == 0 ) {
          if( m_debug ) printf("failure...\n");
//           j_error ("sasl authentication failed");
//           break;
        } else if ( strcmp ( "success", iks_name ( node ) ) == 0 ) {
          if( m_debug ) printf( "sasl initialisation successful...\n" );
          m_authorized = true;
          header( server() );
        } else {
          ikspak* pak;
          pak = iks_packet ( node );
          iks_filter_packet ( m_filter, pak );
/*          if (sess->job_done == 1) return IKS_HOOK;*/
        }
        break;
    case IKS_NODE_ERROR:      // <stream:error>
//       syslog(LOG_ERR, "stream error. quitting\n");
//       logout();
      break;
    case IKS_NODE_STOP:       // </stream:stream>
      break;
  }
  if( m_debug ) printf("at the end of on_stream\n");
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

void JClient::login( char* sid )
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
                      IKS_RULE_ID, "auth",
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
    if( m_debug ) printf("connection failure\n");
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
}

void JClient::disconnect()
{
  m_thread->cancel();
  Stream::disconnect();
  m_state = STATE_DISCONNECTED;
  delete( m_thread );
  m_thread = 0;
}

void JClient::send( iks* x )
{
  Stream::send( this->P, x );
}

void JClient::send( const char* jid, const char* data )
{
  iks* x = iks_make_msg( IKS_TYPE_NONE, jid, data );
  Stream::send( this->P, x );
}

JClient::state JClient::clientState()
{
  return m_state;
}

void JClient::setClientState( state s )
{
  m_state = s;
}

iksparser* JClient::parser()
{
  return this->P;
}

void JClient::registerPresenceHandler( PresenceHandler* ph )
{
  m_presenceHandlers.push_back( ph );
}

void JClient::removePresenceHandler( PresenceHandler* ph )
{
  m_presenceHandlers.remove( ph );
}

void JClient::registerIqHandler( IqHandler* ih )
{
  m_iqHandlers.push_back( ih );
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
}

void JClient::notifyMessageHandlers( iksid* from, iksubtype type, const char* msg )
{
  MessageHandlerList::const_iterator it = m_messageHandlers.begin();
  for( it; it != m_messageHandlers.end(); it++ ) {
    (*it)->handleMessage( from, type, msg );
  }
}

void authHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("authHook\n");
  if( stream->autoPresence() )
  {
    iks* x = iks_make_pres( IKS_SHOW_AVAILABLE, "online" );
    stream->send( x );
  }

  stream->notifyOnConnect();
}

void registerHook( JClient* stream, ikspak* pak )
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
//     iks_insert_attrib(y, "xmlns", "jabber:iq:register");
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
}

void registeredHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("registeredHook\n");
  stream->setClientState( JClient::STATE_AUTHENTICATED );
}

void msgHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("msgHook\n");
  stream->notifyMessageHandlers( pak->from, pak->subtype, iks_find_cdata( pak->x, "body" ) );
}

void iqHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("iqHook\n");
  stream->notifyIqHandlers( pak->ns, pak );
}

void presenceHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("presenceHook\n");
  stream->notifyPresenceHandlers( pak->from, pak->subtype, pak->show, iks_find_cdata( pak->x, "status" ) );
}

void subscriptionHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("subscriptionHook\n");
  stream->notifySubscriptionHandlers( pak->from, pak->subtype, iks_find_cdata( pak->x, "status" ) );
}

void errorHook( JClient* stream, ikspak* pak )
{
  if( stream->debug() ) printf("errorHook\n");
}
