/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



#include "config.h"

#include "clientbase.h"
#include "connection.h"
#include "parser.h"
#include "tag.h"
#include "stanza.h"

#include <iksemel.h>

#include <string>
#include <map>
#include <list>

#include <math.h>

namespace gloox
{

  ClientBase::ClientBase( const std::string& ns )
  : m_namespace( ns ), m_port( -1 ),
      m_connection( 0 ), m_parser( 0 ),
      m_tls( true ), m_sasl( true ), m_idCount( 0 )
  {
  }

  ClientBase::ClientBase( const std::string& ns, const std::string& password, int port )
    : m_namespace( ns ), m_password( password ), m_port( port ),
      m_connection( 0 ), m_parser( 0 ),
      m_tls( true ), m_sasl( true ), m_idCount( 0 )
  {
  }

  ClientBase::ClientBase( const std::string& ns, const std::string& password,
                          const std::string& server, int port )
    : m_namespace( ns ), m_password( password ), m_port( port ),
      m_connection( 0 ), m_parser( 0 ),
      m_tls( true ), m_sasl( true ), m_idCount( 0 )
  {
    m_jid.setServer( server );
  }

  ClientBase::~ClientBase()
  {
    delete m_connection;
    delete m_parser;
  }

  bool ClientBase::connect()
  {
    if( m_jid.server().empty() )
      return false;

    if( !m_parser )
      m_parser = new Parser( this );

    if( !m_connection )
      m_connection = new Connection( m_parser, m_jid.server(), m_port );

    m_connection->setCACerts( m_cacerts );
    int ret = m_connection->connect();
    if( ret == STATE_CONNECTED )
    {
      header();
      m_connection->receive();
    }

    return true;
  }

  void ClientBase::filter( NodeType type, Stanza *stanza )
  {
//     if( !stanza )
//       return;

#ifdef DEBUG
    if( stanza )
      log( stanza->xml(), true );
#endif

    switch( type )
    {
      case NODE_STREAM_START:
        m_sid = stanza->findAttribute( "id" );
        handleStartNode();
        break;
      case NODE_STREAM_CHILD:
        if( !handleNormalNode( stanza ) )
        {
          switch( stanza->type() )
          {
            case STANZA_IQ:
              notifyIqHandlers( stanza );
              break;
            case STANZA_PRESENCE:
              notifyPresenceHandlers( stanza );
              break;
            case STANZA_S10N:
              notifySubscriptionHandlers( stanza );
              break;
            case STANZA_MESSAGE:
              notifyMessageHandlers( stanza );
              break;
            default:
              notifyTagHandlers( stanza );
              break;
          }
        }
        break;
      case NODE_STREAM_ERROR:
#ifdef DEBUG
        printf( "stream error received\n" );
#endif
        disconnect( STATE_ERROR );
        break;
      case NODE_STREAM_CLOSE:
#ifdef DEBUG
        printf( "stream closed\n" );
#endif
        disconnect( STATE_DISCONNECTED );
        break;
    }
  }

  void ClientBase::disconnect( ConnectionState reason )
  {
    if( m_connection )
    {
      m_connection->setState( reason );
      m_connection->disconnect();
    }
  }

  void ClientBase::header()
  {
    std::string xml = "<?xml version='1.0'?>";
    xml += "<stream:stream to='" + streamTo()+  "' xmlns='" + m_namespace + "' ";
    xml += "xmlns:stream='http://etherx.jabber.org/streams' ";
    xml += "version='1.0'>";
    send( xml );
  }

  bool ClientBase::hasTls()
  {
#ifdef HAVE_GNUTLS
    return true;
#else
    return false;
#endif
  }

#ifdef HAVE_GNUTLS
  void ClientBase::startTls()
  {
    Tag *start = new Tag( "starttls" );
    start->addAttrib( "xmlns", XMLNS_STREAM_TLS );
    send( start );
  }
#endif

  void ClientBase::startSASL( SaslMechanisms type )
  {
    Tag *a = new Tag( "auth" );
    a->addAttrib( "xmlns", XMLNS_STREAM_SASL );

    switch( type )
    {
      case SASL_DIGEST_MD5:
        a->addAttrib( "mechanism", "DIGEST-MD5" );
        break;
      case SASL_PLAIN:
        a->addAttrib( "mechanism", "PLAIN" );
        int len = m_jid.username().length() + m_password.length() + 2;
        char *tmp = (char*)iks_malloc( len + 80 );
        char *result;
        sprintf( tmp, "%c%s%c%s", 0, m_jid.username().c_str(), 0, m_password.c_str() );
        result = iks_base64_encode( tmp, len );
        a->setCData( result );
        iks_free( result );
        iks_free( tmp );
        break;
    }

    send( a );
  }

  void ClientBase::processSASLChallenge( const std::string& challenge )
  {
    const int CNONCE_LEN = 4;
    Tag *t;
    std::string decoded, nonce, realm, response;
    char *b = iks_base64_decode( challenge.c_str() );
    if( b )
      decoded = b;
    else
      return;

    if( decoded.substr( 0, 7 ) == "rspauth" )
    {
      t = new Tag( "response" );
    }
    else
    {
      char cnonce[CNONCE_LEN*8 + 1];
      unsigned char a1_h[16];
      char a1[33], a2[33], response_value[33];
      char *response_coded;
      iksmd5 *md5;
      int i;

      int r_pos = decoded.find( "realm=" );
      if( r_pos != std::string::npos )
      {
        int r_end = decoded.find( "\"", r_pos + 7 );
        realm = decoded.substr( r_pos + 7, r_end - (r_pos + 7 ) );
      }
      else
        realm = m_jid.server();

      int n_pos = decoded.find( "nonce=" );
      if( n_pos != std::string::npos )
      {
        int n_end = decoded.find( "\"", n_pos + 7 );
        while( decoded.substr( n_end-1, 1 ) == "\\" )
          n_end = decoded.find( "\"", n_end + 1 );
        nonce = decoded.substr( n_pos + 7, n_end - ( n_pos + 7 ) );
      }
      else
      {
        iks_free( b );
        return;
      }

      for( i=0; i<CNONCE_LEN; ++i )
        sprintf( cnonce + i*8, "%08x", rand() );

      md5 = iks_md5_new();
      iks_md5_hash( md5, (const unsigned char*)m_jid.username().c_str(), m_jid.username().length(), 0 );
      iks_md5_hash( md5, (const unsigned char*)":", 1, 0 );
      iks_md5_hash( md5, (const unsigned char*)realm.c_str(), realm.length(), 0 );
      iks_md5_hash( md5, (const unsigned char*)":", 1, 0 );
      iks_md5_hash( md5, (const unsigned char*)m_password.c_str(), m_password.length(), 1 );
      iks_md5_digest( md5, a1_h );
      iks_md5_reset( md5 );
      iks_md5_hash( md5, a1_h, 16, 0 );
      iks_md5_hash( md5, (const unsigned char*)":", 1, 0 );
      iks_md5_hash( md5, (const unsigned char*)nonce.c_str(), nonce.length(), 0 );
      iks_md5_hash( md5, (const unsigned char*)":", 1, 0 );
      iks_md5_hash( md5, (const unsigned char*)cnonce, iks_strlen( cnonce ), 1 );
      iks_md5_print( md5, a1 );
      iks_md5_reset( md5 );
      iks_md5_hash( md5, (const unsigned char*)"AUTHENTICATE:xmpp/", 18, 0 );
      iks_md5_hash( md5, (const unsigned char*)m_jid.server().c_str(), m_jid.server().length(), 1 );
      iks_md5_print( md5, a2 );
      iks_md5_reset( md5 );
      iks_md5_hash( md5, (const unsigned char*)a1, 32, 0 );
      iks_md5_hash( md5, (const unsigned char*)":", 1, 0 );
      iks_md5_hash( md5, (const unsigned char*)nonce.c_str(), nonce.length(), 0 );
      iks_md5_hash( md5, (const unsigned char*)":00000001:", 10, 0 );
      iks_md5_hash( md5, (const unsigned char*)cnonce, iks_strlen( cnonce ), 0 );
      iks_md5_hash( md5, (const unsigned char*)":auth:", 6, 0 );
      iks_md5_hash( md5, (const unsigned char*)a2, 32, 1 );
      iks_md5_print( md5, response_value );
      iks_md5_delete( md5 );

      i = m_jid.username().length() + realm.length() +
          nonce.length() + m_jid.server().length() +
          CNONCE_LEN*8 + 136;

      std::string response = "username=\"" + m_jid.username() + "\",realm=\"" + realm;
      response += "\",nonce=\""+ nonce + "\",cnonce=\"";
      response += cnonce;
      response += "\",nc=00000001,qop=auth,digest-uri=\"xmpp/" + m_jid.server() + "\",response=";
      response += response_value;
      response += ",charset=utf-8";
      response_coded = iks_base64_encode( response.c_str(), response.length() );

      t = new Tag( "response", response_coded );

      iks_free( response_coded );
    }
    t->addAttrib( "xmlns", XMLNS_STREAM_SASL );
    send( t );
    iks_free( b );


//     disconnect( STATE_DISCONNECTED );
  }

  void ClientBase::send( Tag *tag )
  {
#ifdef DEBUG
    log( tag->xml(), false );
#endif
    if( m_connection )
      m_connection->send( tag->xml() );

    if( tag->type() == STANZA_UNDEFINED )
      delete( tag );
    else
    {
      Stanza *s = dynamic_cast<Stanza*>( tag );
      delete( s );
    }
  }

  void ClientBase::send( const std::string& xml )
  {
#ifdef DEBUG
    log( xml, false );
#endif
    if( m_connection )
      m_connection->send( xml );
  }

  ConnectionState ClientBase::state() const{
    if( m_connection )
      return m_connection->state();
    else
      return STATE_DISCONNECTED;
  }

  void ClientBase::setState( ConnectionState state )
  {
    if( m_connection )
      m_connection->setState( state );
  }

  const std::string ClientBase::getID()
  {
    char tmp[4+(int)log10(++m_idCount)+1];
    sprintf( tmp, "uid%d", m_idCount );
    return tmp;
  }

  void ClientBase::log( const std::string& xml, bool incoming )
  {
    if ( m_connection->isSecure() )
      printf( "Sec" );

    if( incoming )
      printf( "RECV " );
    else
      printf( "SEND " );

    printf( "[%s]", xml.c_str() );
    if( xml.substr( xml.length()-2, 1 ) != "\n" )
      printf( "\n" );
  }

  void ClientBase::registerPresenceHandler( PresenceHandler *ph )
  {
    m_presenceHandlers.push_back( ph );
  }

  void ClientBase::removePresenceHandler( PresenceHandler *ph )
  {
    m_presenceHandlers.remove( ph );
  }

  void ClientBase::registerIqHandler( IqHandler *ih, const std::string& xmlns )
  {
    m_iqNSHandlers[xmlns] = ih;
  }

  void ClientBase::trackID( IqHandler *ih, const std::string& id, int context )
  {
    TrackStruct track;
    track.ih = ih;
    track.context = context;
    m_iqIDHandlers[id] = track;
  }

  void ClientBase::removeIqHandler( const std::string& xmlns )
  {
    m_iqNSHandlers.erase( xmlns );
  }

  void ClientBase::registerMessageHandler( MessageHandler *mh )
  {
    m_messageHandlers.push_back( mh );
  }

  void ClientBase::removeMessageHandler( MessageHandler *mh )
  {
    m_messageHandlers.remove( mh );
  }

  void ClientBase::registerSubscriptionHandler( SubscriptionHandler *sh )
  {
    m_subscriptionHandlers.push_back( sh );
  }

  void ClientBase::registerTagHandler( TagHandler *th, const std::string& tag, const std::string& xmlns )
  {
    TagHandlerStruct ths;
    ths.tag = tag;
    ths.xmlns = xmlns;
    ths.th = th;
    m_tagHandlers.push_back( ths );
  }

  void ClientBase::removeSubscriptionHandler( SubscriptionHandler *sh )
  {
    m_subscriptionHandlers.remove( sh );
  }

  void ClientBase::registerConnectionListener( ConnectionListener *cl )
  {
    m_connectionListeners.push_back( cl );
  }

  void ClientBase::removeTagHandler( TagHandler *th, const std::string& tag, const std::string& xmlns )
  {
    TagHandlerList::iterator it = m_tagHandlers.begin();
    for( it; it != m_tagHandlers.end(); ++it )
    {
      if( (*it).th == th && (*it).tag == tag && (*it).xmlns == xmlns )
        m_tagHandlers.erase( it );
    }
  }

  void ClientBase::notifyOnConnect()
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( it; it != m_connectionListeners.end(); it++ )
    {
      (*it)->onConnect();
    }
  }

  void ClientBase::notifyOnDisconnect()
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( it; it != m_connectionListeners.end(); it++ )
    {
      (*it)->onDisconnect();
    }
  }

  bool ClientBase::notifyOnTLSConnect( const CertInfo& info )
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( it; it != m_connectionListeners.end(); it++ )
    {
      return (*it)->onTLSConnect( info );
    }
  }

  void ClientBase::notifyOnResourceBindError( ConnectionListener::ResourceBindError error )
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( it; it != m_connectionListeners.end(); it++ )
    {
      (*it)->onResourceBindError( error );
    }
  }

  void ClientBase::notifyOnSessionCreateError( ConnectionListener::SessionCreateError error )
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( it; it != m_connectionListeners.end(); it++ )
    {
      (*it)->onSessionCreateError( error );
    }
  }

  void ClientBase::notifyPresenceHandlers( Stanza *stanza )
  {
    PresenceHandlerList::const_iterator it = m_presenceHandlers.begin();
    for( it; it != m_presenceHandlers.end(); it++ )
    {
      (*it)->handlePresence( stanza );
    }
  }

  void ClientBase::notifySubscriptionHandlers( Stanza *stanza )
  {
    SubscriptionHandlerList::const_iterator it = m_subscriptionHandlers.begin();
    for( it; it != m_subscriptionHandlers.end(); it++ )
    {
      (*it)->handleSubscription( stanza );
    }
  }

  void ClientBase::notifyIqHandlers( Stanza *stanza )
  {
    bool res = false;

    IqHandlerMap::const_iterator it_ns = m_iqNSHandlers.begin();
    for( it_ns; it_ns != m_iqNSHandlers.end(); it_ns++ )
    {
      if( stanza->hasChildWithAttrib( "xmlns", (*it_ns).first ) )
      {
        if( (*it_ns).second->handleIq( stanza ) )
          res = true;
      }
    }

    IqTrackMap::iterator it_id = m_iqIDHandlers.find( stanza->id() );
    if( it_id != m_iqIDHandlers.end() )
    {
      if( (*it_id).second.ih->handleIqID( stanza, (*it_id).second.context ) )
        res = true;
      m_iqIDHandlers.erase( it_id );
    }

    if( !res && ( stanza->type() == STANZA_IQ ) &&
         ( ( stanza->subtype() == STANZA_IQ_GET ) || ( stanza->subtype() == STANZA_IQ_SET ) ) )
    {
      Tag *iq = new Tag( "iq" );
      iq->addAttrib( "type", "error" );
      iq->addAttrib( "id", stanza->id() );
      iq->addAttrib( "to", stanza->from().full() );
      send( iq );
    }
  }

  void ClientBase::notifyMessageHandlers( Stanza *stanza )
  {
    MessageHandlerList::const_iterator it = m_messageHandlers.begin();
    for( it; it != m_messageHandlers.end(); it++ )
    {
      (*it)->handleMessage( stanza );
    }
  }

  void ClientBase::notifyTagHandlers( Stanza *stanza )
  {
    TagHandlerList::const_iterator it = m_tagHandlers.begin();
    for( it; it != m_tagHandlers.end(); it++ )
    {
      if( (*it).tag == stanza->name() && (*it).xmlns == stanza->xmlns() )
        (*it).th->handleTag( stanza );
    }
  }

};
