/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/



#ifdef WIN32
#include "../config.h.win"
#else
#include "config.h"
#endif

#include "clientbase.h"
#include "connection.h"
#include "logsink.h"
#include "parser.h"
#include "tag.h"
#include "stanza.h"

#include <iksemel.h>

#include <string>
#include <map>
#include <list>
#include <sstream>

namespace gloox
{

  ClientBase::ClientBase( const std::string& ns, const std::string& server, int port )
    : m_connection( 0 ), m_namespace( ns ), m_xmllang( "en" ), m_server( server ),
      m_authed( false ), m_sasl( true ), m_tls( true ), m_port( port ), m_parser( 0 ),
      m_authError( AUTH_ERROR_UNDEFINED ), m_streamError( ERROR_UNDEFINED ),
      m_streamErrorAppCondition( 0 ), m_idCount( 0 )
  {
  }

  ClientBase::ClientBase( const std::string& ns, const std::string& password,
                          const std::string& server, int port )
    : m_connection( 0 ), m_namespace( ns ), m_password( password ), m_xmllang( "en" ), m_server( server ),
      m_authed( false ), m_sasl( true ), m_tls( true ), m_port( port ), m_parser( 0 ),
      m_authError( AUTH_ERROR_UNDEFINED ), m_streamError( ERROR_UNDEFINED ),
      m_streamErrorAppCondition( 0 ), m_idCount( 0 )
  {
  }

  ClientBase::~ClientBase()
  {
    delete m_connection;
    delete m_parser;
  }

  ConnectionError ClientBase::recv( int timeout )
  {
    ConnectionError e = m_connection->recv( timeout );
    if( e != CONN_OK )
      notifyOnDisconnect( e );
    return e;
  }

  bool ClientBase::connect( bool block )
  {
    if( m_server.empty() )
      return false;

    if( !m_parser )
      m_parser = new Parser( this );

    if( !m_connection )
      m_connection = new Connection( m_parser, m_server, m_port );

#ifdef HAVE_GNUTLS
    m_connection->setCACerts( m_cacerts );
#endif
    int ret = m_connection->connect();
    if( ret == STATE_CONNECTED )
    {
      header();
      if( block )
      {
        ConnectionError e = m_connection->receive();
        notifyOnDisconnect( e );
        return false;
      }
      else
        return true;
    }
    else
      return false;
  }

  void ClientBase::filter( NodeType type, Stanza *stanza )
  {
    if( stanza )
      LogSink::instance().log( LOG_DEBUG, LOG_XML_INCOMING, stanza->xml() );

    switch( type )
    {
      case NODE_STREAM_START:
      {
        const std::string version = stanza->findAttribute( "version" );
        if( !version.empty() )
        {
          if( !checkStreamVersion( version ) )
            disconnect( CONN_STREAM_ERROR );
        }
        else
        {
          printf( "This server is not XMPP-compliant (it does not send a 'version' attribute). Plase try another one.\n" );
              disconnect( CONN_STREAM_ERROR );
        }

        m_sid = stanza->findAttribute( "id" );
        handleStartNode();
        break;
      }
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
        handleStreamError( stanza );
        break;
      case NODE_STREAM_CLOSE:
        LogSink::instance().log( LOG_DEBUG, LOG_CLASS_CLIENTBASE, "stream closed" );
        disconnect( CONN_STREAM_CLOSED );
        break;
    }
  }

  void ClientBase::disconnect()
  {
    disconnect( CONN_USER_DISCONNECTED );
  }

  void ClientBase::disconnect( ConnectionError reason )
  {
    if( m_connection )
    {
      if( reason == CONN_USER_DISCONNECTED )
        m_streamError = ERROR_UNDEFINED;
      m_connection->disconnect( reason );
    }
  }

  void ClientBase::header()
  {
    std::string xml = "<?xml version='1.0' ?>";
    xml += "<stream:stream to='" + m_jid.server()+ "' xmlns='" + m_namespace + "' ";
    xml += "xmlns:stream='http://etherx.jabber.org/streams'  xml:lang='" + m_xmllang + "' ";
    xml += "version='";
    xml += XMPP_STREAM_VERSION_MAJOR;
    xml += ".";
    xml += XMPP_STREAM_VERSION_MINOR;
    xml += "'>";
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
      {
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
      case SASL_ANONYMOUS:
        a->addAttrib( "mechanism", "ANONYMOUS" );
        a->setCData( getID() );
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

      unsigned int r_pos = decoded.find( "realm=" );
      if( r_pos != std::string::npos )
      {
        unsigned int r_end = decoded.find( "\"", r_pos + 7 );
        realm = decoded.substr( r_pos + 7, r_end - (r_pos + 7 ) );
      }
      else
        realm = m_jid.server();

      unsigned int n_pos = decoded.find( "nonce=" );
      if( n_pos != std::string::npos )
      {
        unsigned int n_end = decoded.find( "\"", n_pos + 7 );
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

  }

  void ClientBase::processSASLError( Stanza *stanza )
  {
    if( stanza->hasChild( "aborted" ) )
      m_authError = SASL_ABORTED;
    else if( stanza->hasChild( "incorrect-encoding" ) )
      m_authError = SASL_INCORRECT_ENCODING;
    else if( stanza->hasChild( "invalid-authzid" ) )
      m_authError = SASL_INVALID_AUTHZID;
    else if( stanza->hasChild( "invalid-mechanism" ) )
      m_authError = SASL_INVALID_MECHANISM;
    else if( stanza->hasChild( "mechanism-too-weak" ) )
      m_authError = SASL_MECHANISM_TOO_WEAK;
    else if( stanza->hasChild( "not-authorized" ) )
      m_authError = SASL_NOT_AUTHORIZED;
    else if( stanza->hasChild( "temporary-auth-failure" ) )
      m_authError = SASL_TEMPORARY_AUTH_FAILURE;
  }

  void ClientBase::send( Tag *tag )
  {
    if( !tag )
      return;

    send( tag->xml() );

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
    LogSink::instance().log(LOG_DEBUG, LOG_XML_OUTGOING, xml );

    if( m_connection )
      m_connection->send( xml );
  }

  ConnectionState ClientBase::state() const{
    if( m_connection )
      return m_connection->state();
    else
      return STATE_DISCONNECTED;
  }

  const std::string ClientBase::getID()
  {
    std::ostringstream oss;
    oss << ++m_idCount;
    return std::string( "uid" ) + oss.str();
  }

  bool ClientBase::checkStreamVersion( const std::string& version )
  {
    int major = 0;
    int minor = 0;
    int myMajor = atoi( XMPP_STREAM_VERSION_MAJOR );

    unsigned int dot = version.find( "." );
    if( !version.empty() && dot && dot != std::string::npos )
    {
      major = atoi( version.substr( 0, dot ).c_str() );
      minor = atoi( version.substr( dot ).c_str() );
    }

    if( myMajor < major )
      return false;
    else
      return true;
  }

  void ClientBase::handleStreamError( Stanza *stanza )
  {
    Tag::TagList& c = stanza->children();
    Tag::TagList::const_iterator it = c.begin();
    for( ; it != c.end(); ++it )
    {
      if( (*it)->name() == "bad-format" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_BAD_FORMAT;
      else if( (*it)->name() == "bad-namespace-prefix" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_BAD_NAMESPACE_PREFIX;
      else if( (*it)->name() == "conflict" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_CONFLICT;
      else if( (*it)->name() == "connection-timeout" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_CONNECTION_TIMEOUT;
      else if( (*it)->name() == "host-gone" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_HOST_GONE;
      else if( (*it)->name() == "host-unknown" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_HOST_UNKNOWN;
      else if( (*it)->name() == "improper-addressing" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_IMPROPER_ADDRESSING;
      else if( (*it)->name() == "internal-server-error" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_INTERNAL_SERVER_ERROR;
      else if( (*it)->name() == "invalid-from" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_INVALID_FROM;
      else if( (*it)->name() == "invalid-id" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_INVALID_ID;
      else if( (*it)->name() == "invalid-namespace" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_INVALID_NAMESPACE;
      else if( (*it)->name() == "invalid-xml" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_INVALID_XML;
      else if( (*it)->name() == "not-authorized" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_NOT_AUTHORIZED;
      else if( (*it)->name() == "policy-violation" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_POLICY_VIOLATION;
      else if( (*it)->name() == "remote-connection-failed" &&
                 (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_REMOTE_CONNECTION_FAILED;
      else if( (*it)->name() == "resource-constraint" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_RESOURCE_CONSTRAINT;
      else if( (*it)->name() == "restricted-xml" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_RESTRICTED_XML;
      else if( (*it)->name() == "see-other-host" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
      {
        m_streamError = ERROR_SEE_OTHER_HOST;
        m_streamErrorCData = stanza->findChild( "see-other-host" )->cdata();
      }
      else if( (*it)->name() == "system-shutdown" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_SYSTEM_SHUTDOWN;
      else if( (*it)->name() == "undefined-condition" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_UNDEFINED_CONDITION;
      else if( (*it)->name() == "unsupported-encoding" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_UNSUPPORTED_ENCODING;
      else if( (*it)->name() == "unsupported-stanza-type" &&
                 (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_UNSUPPORTED_STANZA_TYPE;
      else if( (*it)->name() == "unsupported-version" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_UNSUPPORTED_VERSION;
      else if( (*it)->name() == "xml-not-well-formed" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = ERROR_XML_NOT_WELL_FORMED;
      else if( (*it)->name() == "text" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
      {
        const std::string lang = (*it)->findAttribute( "xml:lang" );
        if( !lang.empty() )
          m_streamErrorText[lang] = (*it)->cdata();
        else
          m_streamErrorText["default"] = (*it)->cdata();
      }
      else
        m_streamErrorAppCondition = (*it);
    }

    disconnect( CONN_STREAM_ERROR );
  }

  const std::string ClientBase::streamErrorText( const std::string& lang ) const
  {
    StringMap::const_iterator it = m_streamErrorText.find( lang );
    if( it != m_streamErrorText.end() )
      return (*it).second;
    else
      return "";
  }

  int ClientBase::fileDescriptor() const
  {
    if( m_connection )
      return m_connection->fileDescriptor();
    else
      return -1;
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

  void ClientBase::removeConnectionListener( ConnectionListener *cl )
  {
    m_connectionListeners.remove( cl );
  }

  void ClientBase::removeTagHandler( TagHandler *th, const std::string& tag, const std::string& xmlns )
  {
    TagHandlerList::iterator it = m_tagHandlers.begin();
    for( ; it != m_tagHandlers.end(); ++it )
    {
      if( (*it).th == th && (*it).tag == tag && (*it).xmlns == xmlns )
        m_tagHandlers.erase( it );
    }
  }

  void ClientBase::notifyOnConnect()
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( ; it != m_connectionListeners.end(); ++it )
    {
      (*it)->onConnect();
    }
  }

  void ClientBase::notifyOnDisconnect( ConnectionError e )
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( ; it != m_connectionListeners.end(); ++it )
    {
      (*it)->onDisconnect( e );
    }
  }

  bool ClientBase::notifyOnTLSConnect( const CertInfo& info )
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( ; it != m_connectionListeners.end(); ++it )
    {
      return (*it)->onTLSConnect( info );
    }

    return false;
  }

  void ClientBase::notifyOnResourceBindError( ConnectionListener::ResourceBindError error )
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( ; it != m_connectionListeners.end(); ++it )
    {
      (*it)->onResourceBindError( error );
    }
  }

  void ClientBase::notifyOnSessionCreateError( ConnectionListener::SessionCreateError error )
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( ; it != m_connectionListeners.end(); ++it )
    {
      (*it)->onSessionCreateError( error );
    }
  }

  void ClientBase::notifyPresenceHandlers( Stanza *stanza )
  {
    PresenceHandlerList::const_iterator it = m_presenceHandlers.begin();
    for( ; it != m_presenceHandlers.end(); ++it )
    {
      (*it)->handlePresence( stanza );
    }
  }

  void ClientBase::notifySubscriptionHandlers( Stanza *stanza )
  {
    SubscriptionHandlerList::const_iterator it = m_subscriptionHandlers.begin();
    for( ; it != m_subscriptionHandlers.end(); ++it )
    {
      (*it)->handleSubscription( stanza );
    }
  }

  void ClientBase::notifyIqHandlers( Stanza *stanza )
  {
    bool res = false;

    IqHandlerMap::const_iterator it = m_iqNSHandlers.begin();
    for( ; it != m_iqNSHandlers.end(); ++it )
    {
      if( stanza->hasChildWithAttrib( "xmlns", (*it).first ) )
      {
        if( (*it).second->handleIq( stanza ) )
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
      iq->addAttrib( "type", "result" );
      iq->addAttrib( "id", stanza->id() );
      iq->addAttrib( "to", stanza->from().full() );
      send( iq );
    }
  }

  void ClientBase::notifyMessageHandlers( Stanza *stanza )
  {
    MessageHandlerList::const_iterator it = m_messageHandlers.begin();
    for( ; it != m_messageHandlers.end(); ++it )
    {
      (*it)->handleMessage( stanza );
    }
  }

  void ClientBase::notifyTagHandlers( Stanza *stanza )
  {
    TagHandlerList::const_iterator it = m_tagHandlers.begin();
    for( ; it != m_tagHandlers.end(); ++it )
    {
      if( (*it).tag == stanza->name() && (*it).xmlns == stanza->xmlns() )
        (*it).th->handleTag( stanza );
    }
  }

}
