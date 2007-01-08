/*
  Copyright (c) 2005-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifdef WIN32
#include "../config.h.win"
#else
#include "config.h"
#endif

#include "clientbase.h"
#include "connection.h"
#include "disco.h"
#include "messagesessionhandler.h"
#include "parser.h"
#include "tag.h"
#include "stanza.h"
#include "connectionlistener.h"
#include "iqhandler.h"
#include "messagehandler.h"
#include "presencehandler.h"
#include "rosterlistener.h"
#include "subscriptionhandler.h"
#include "loghandler.h"
#include "taghandler.h"
#include "mucinvitationhandler.h"
#include "jid.h"
#include "base64.h"
#include "md5.h"

#include <string>
#include <map>
#include <list>
#include <sstream>
#include <iomanip>

namespace gloox
{

  ClientBase::ClientBase( const std::string& ns, const std::string& server, int port )
    : m_connection( 0 ), m_disco( 0 ), m_namespace( ns ),
      m_xmllang( "en" ), m_server( server ),
      m_compression( true ), m_authed( false ), m_sasl( true ), m_tls( true ), m_port( port ),
      m_statisticsHandler( 0 ), m_mucInvitationHandler( 0 ),
      m_messageSessionHandlerChat( 0 ), m_messageSessionHandlerGroupchat( 0 ),
      m_messageSessionHandlerHeadline( 0 ), m_messageSessionHandlerNormal( 0 ),
      m_parser( 0 ), m_authError( AuthErrorUndefined ), m_streamError( StreamErrorUndefined ),
      m_streamErrorAppCondition( 0 ), m_idCount( 0 ), m_autoMessageSession( false ),
      m_fdRequested( false )
  {
    init();
  }

  ClientBase::ClientBase( const std::string& ns, const std::string& password,
                          const std::string& server, int port )
    : m_connection( 0 ), m_disco( 0 ), m_namespace( ns ), m_password( password ),
      m_xmllang( "en" ), m_server( server ),
      m_compression( true ), m_authed( false ), m_sasl( true ), m_tls( true ), m_port( port ),
      m_statisticsHandler( 0 ), m_mucInvitationHandler( 0 ),
      m_messageSessionHandlerChat( 0 ), m_messageSessionHandlerGroupchat( 0 ),
      m_messageSessionHandlerHeadline( 0 ), m_messageSessionHandlerNormal( 0 ),
      m_parser( 0 ), m_authError( AuthErrorUndefined ), m_streamError( StreamErrorUndefined ),
      m_streamErrorAppCondition( 0 ), m_idCount( 0 ), m_autoMessageSession( false ),
      m_fdRequested( false )
  {
    init();
  }

  void ClientBase::init()
  {
    m_disco = new Disco( this );
    m_disco->setVersion( "based on gloox", GLOOX_VERSION );

    m_stats.totalBytesSent = 0;
    m_stats.totalBytesReceived = 0;
    m_stats.compressedBytesSent = 0;
    m_stats.compressedBytesReceived = 0;
    m_stats.uncompressedBytesSent = 0;
    m_stats.uncompressedBytesReceived = 0;
    m_stats.totalStanzasSent = 0;
    m_stats.totalStanzasReceived = 0;
    m_stats.iqStanzasSent = 0;
    m_stats.iqStanzasReceived = 0;
    m_stats.messageStanzasSent = 0;
    m_stats.messageStanzasReceived = 0;
    m_stats.s10nStanzasSent = 0;
    m_stats.s10nStanzasReceived = 0;
    m_stats.presenceStanzasSent = 0;
    m_stats.presenceStanzasReceived = 0;
    m_stats.encryption = false;
    m_stats.compression = false;
  }

  ClientBase::~ClientBase()
  {
    delete m_connection;
    delete m_parser;
    delete m_disco;
  }

  ConnectionError ClientBase::recv( int timeout )
  {
    if( !m_connection || m_connection->state() == StateDisconnected )
      return ConnNotConnected;

    ConnectionError e = m_connection->recv( timeout );
    if( e != ConnNoError )
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
      m_connection = new Connection( m_parser, m_logInstance, m_server, m_port );

#ifdef HAVE_TLS
    m_connection->setCACerts( m_cacerts );
    if( !m_clientKey.empty() && !m_clientCerts.empty() )
      m_connection->setClientCert( m_clientKey, m_clientCerts );
#endif

    if( m_connection->state() >= StateConnecting )
      return true;

    ConnectionError ret = m_connection->connect();
    if( ret == ConnNoError )
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
    {
      notifyOnDisconnect( ret );
      return false;
    }
  }

  void ClientBase::handleStanza( NodeType type, Stanza *stanza )
  {
    if( !stanza )
      return;

    logInstance().log( LogLevelDebug, LogAreaXmlIncoming, stanza->xml() );
    ++m_stats.totalStanzasReceived;

    switch( type )
    {
      case NodeStreamStart:
      {
        const std::string version = stanza->findAttribute( "version" );
        if( !checkStreamVersion( version ) )
        {
          logInstance().log( LogLevelDebug, LogAreaClassClientbase, "This server is not XMPP-compliant"
              " (it does not send a 'version' attribute). Please fix it or try another one.\n" );
          disconnect( ConnStreamError );
        }

        m_sid = stanza->findAttribute( "id" );
        handleStartNode();
        break;
      }
      case NodeStreamChild:
        if( !handleNormalNode( stanza ) )
        {
          switch( stanza->type() )
          {
            case StanzaIq:
              notifyIqHandlers( stanza );
              ++m_stats.iqStanzasReceived;
              break;
            case StanzaPresence:
              notifyPresenceHandlers( stanza );
              ++m_stats.presenceStanzasReceived;
              break;
            case StanzaS10n:
              notifySubscriptionHandlers( stanza );
              ++m_stats.s10nStanzasReceived;
              break;
            case StanzaMessage:
              notifyMessageHandlers( stanza );
              ++m_stats.messageStanzasReceived;
              break;
            default:
              notifyTagHandlers( stanza );
              break;
          }
        }
        break;
      case NodeStreamError:
        handleStreamError( stanza );
        disconnect( ConnStreamError );
        break;
      case NodeStreamClose:
        logInstance().log( LogLevelDebug, LogAreaClassClientbase, "stream closed" );
        disconnect( ConnStreamClosed );
        break;
    }

    if( m_statisticsHandler )
      m_statisticsHandler->handleStatistics( getStatistics() );
  }

  void ClientBase::disconnect( ConnectionError reason )
  {
    if( m_connection )
    {
      if( reason != ConnStreamError )
       send ( "</stream:stream>" );
      if( reason == ConnUserDisconnected )
        m_streamError = StreamErrorUndefined;
      m_connection->disconnect( reason );
    }
  }

  void ClientBase::header()
  {
    std::ostringstream oss;
    oss << "<?xml version='1.0' ?>";
    oss << "<stream:stream to='" + m_jid.server()+ "' xmlns='" + m_namespace + "' ";
    oss << "xmlns:stream='http://etherx.jabber.org/streams'  xml:lang='" + m_xmllang + "' ";
    oss << "version='";
    oss << XMPP_STREAM_VERSION_MAJOR;
    oss << ".";
    oss << XMPP_STREAM_VERSION_MINOR;
    oss << "'>";
    send( oss.str() );
  }

  bool ClientBase::hasTls()
  {
#ifdef HAVE_TLS
    return true;
#else
    return false;
#endif
  }

#ifdef HAVE_TLS
  void ClientBase::startTls()
  {
    Tag *start = new Tag( "starttls" );
    start->addAttribute( "xmlns", XMLNS_STREAM_TLS );
    send( start );
  }

  void ClientBase::setClientCert( const std::string& clientKey, const std::string& clientCerts )
  {
    m_clientKey = clientKey;
    m_clientCerts = clientCerts;
  }
#endif

  void ClientBase::startSASL( SaslMechanisms type )
  {
    Tag *a = new Tag( "auth" );
    a->addAttribute( "xmlns", XMLNS_STREAM_SASL );

    switch( type )
    {
      case SaslDigestMd5:
        a->addAttribute( "mechanism", "DIGEST-MD5" );
        break;
      case SaslPlain:
      {
        a->addAttribute( "mechanism", "PLAIN" );

        size_t len = 0;
        if( m_authzid.empty() )
          len = m_jid.username().length() + m_password.length() + 2;
        else
          len = m_authzid.bare().length() + m_jid.username().length() + m_password.length() + 2;

        char *tmp = (char*)malloc( len + 80 );

        if( m_authzid.empty() )
          sprintf( tmp, "%c%s%c%s", 0, m_jid.username().c_str(), 0, m_password.c_str() );
        else
          sprintf( tmp, "%s%c%s%c%s", m_authzid.bare().c_str(), 0, m_jid.username().c_str(), 0,
                   m_password.c_str() );

        std::string dec;
        dec.assign( tmp, len );
        a->setCData( Base64::encode64( dec ) );
        free( tmp );
        break;
      }
      case SaslAnonymous:
        a->addAttribute( "mechanism", "ANONYMOUS" );
        a->setCData( getID() );
        break;
      case SaslExternal:
        a->addAttribute( "mechanism", "EXTERNAL" );
        if( m_authzid.empty() )
          a->setCData( Base64::encode64( m_jid.bare() ) );
        else
          a->setCData( Base64::encode64( m_authzid.bare() ) );
        break;
    }

    send( a );
  }

  void ClientBase::processSASLChallenge( const std::string& challenge )
  {
    Tag *t;
    std::string decoded, nonce, realm, response;
    decoded = Base64::decode64( challenge.c_str() );

    if( decoded.substr( 0, 7 ) == "rspauth" )
    {
      t = new Tag( "response" );
    }
    else
    {
      size_t r_pos = decoded.find( "realm=" );
      if( r_pos != std::string::npos )
      {
        size_t r_end = decoded.find( "\"", r_pos + 7 );
        realm = decoded.substr( r_pos + 7, r_end - (r_pos + 7 ) );
      }
      else
        realm = m_jid.server();

      size_t n_pos = decoded.find( "nonce=" );
      if( n_pos != std::string::npos )
      {
        size_t n_end = decoded.find( "\"", n_pos + 7 );
        while( decoded.substr( n_end-1, 1 ) == "\\" )
          n_end = decoded.find( "\"", n_end + 1 );
        nonce = decoded.substr( n_pos + 7, n_end - ( n_pos + 7 ) );
      }
      else
      {
        return;
      }

      std::ostringstream cnonce;
      for( int i = 0; i < 4; ++i )
        cnonce << std::hex << std::setw( 8 ) << std::setfill( '0' ) << rand();

      std::string a1;
      std::string a2;
      std::string a1_h;
      std::string response_value;
      MD5 md5;
      md5.feed( m_jid.username() );
      md5.feed( ":" );
      md5.feed( realm );
      md5.feed( ":" );
      md5.feed( m_password );
      md5.finalize();
      a1_h = md5.binary();
      md5.reset();
      md5.feed( a1_h );
      md5.feed( ":" );
      md5.feed( nonce );
      md5.feed( ":" );
      md5.feed( cnonce.str() );
      md5.finalize();
      a1  = md5.hex();
      md5.reset();
      md5.feed( "AUTHENTICATE:xmpp/" );
      md5.feed( m_jid.server() );
      md5.finalize();
      a2 = md5.hex();
      md5.reset();
      md5.feed( a1 );
      md5.feed( ":" );
      md5.feed( nonce );
      md5.feed( ":00000001:" );
      md5.feed( cnonce.str() );
      md5.feed( ":auth:" );
      md5.feed( a2 );
      md5.finalize();
      response_value = md5.hex();

      std::string response = "username=\"" + m_jid.username() + "\",realm=\"" + realm;
      response += "\",nonce=\""+ nonce + "\",cnonce=\"";
      response += cnonce.str();
      response += "\",nc=00000001,qop=auth,digest-uri=\"xmpp/" + m_jid.server() + "\",response=";
      response += response_value;
      response += ",charset=utf-8";

      if( !m_authzid.empty() )
        response += ",authzid=" + m_authzid.bare();

      t = new Tag( "response", Base64::encode64( response ) );
    }
    t->addAttribute( "xmlns", XMLNS_STREAM_SASL );
    send( t );
  }

  void ClientBase::processSASLError( Stanza *stanza )
  {
    if( stanza->hasChild( "aborted" ) )
      m_authError = SaslAborted;
    else if( stanza->hasChild( "incorrect-encoding" ) )
      m_authError = SaslIncorrectEncoding;
    else if( stanza->hasChild( "invalid-authzid" ) )
      m_authError = SaslInvalidAuthzid;
    else if( stanza->hasChild( "invalid-mechanism" ) )
      m_authError = SaslInvalidMechanism;
    else if( stanza->hasChild( "mechanism-too-weak" ) )
      m_authError = SaslMechanismTooWeak;
    else if( stanza->hasChild( "not-authorized" ) )
      m_authError = SaslNotAuthorized;
    else if( stanza->hasChild( "temporary-auth-failure" ) )
      m_authError = SaslTemporaryAuthFailure;
  }

  JID& ClientBase::jid()
  {
    if( m_authzid.empty() )
      return m_jid;
    else
      return m_authzid;
  }

  void ClientBase::send( Tag *tag )
  {
    if( !tag )
      return;

    send( tag->xml() );

    switch( tag->type() )
    {
      case StanzaIq:
        ++m_stats.iqStanzasSent;
        break;
      case StanzaMessage:
        ++m_stats.messageStanzasSent;
        break;
      case StanzaS10n:
        ++m_stats.s10nStanzasSent;
        break;
      case StanzaPresence:
        ++m_stats.presenceStanzasSent;
        break;
      default:
        break;
    }
    ++m_stats.totalStanzasSent;

    Stanza *s = dynamic_cast<Stanza*>( tag );
    if( s )
      delete s;
    else
      delete tag;

    if( m_statisticsHandler )
      m_statisticsHandler->handleStatistics( getStatistics() );
  }

  void ClientBase::send( const std::string& xml )
  {
    if( m_connection && m_connection->state() == StateConnected )
    {
      if( m_connection->send( xml ) == false )
        disconnect( ConnStreamError );
      else
        m_stats.totalBytesSent += xml.length();

      logInstance().log( LogLevelDebug, LogAreaXmlOutgoing, xml );
    }
  }

  StatisticsStruct ClientBase::getStatistics()
  {
    if( m_connection )
      m_connection->getStatistics( m_stats.totalBytesReceived, m_stats.totalBytesSent,
                                   m_stats.compressedBytesReceived, m_stats.compressedBytesSent,
                                   m_stats.uncompressedBytesReceived, m_stats.uncompressedBytesSent,
                                   m_stats.compression );
    return m_stats;
  }

  ConnectionState ClientBase::state() const
  {
    if( m_connection )
      return m_connection->state();
    else
      return StateDisconnected;
  }

  void ClientBase::whitespacePing()
  {
    send( " " );
  }

  void ClientBase::xmppPing( const JID& to )
  {
    const std::string id = getID();

    Tag *iq = new Tag( "iq" );
    iq->addAttribute( "to", to.full() );
    iq->addAttribute( "id", id );
    iq->addAttribute( "type", "get" );
    Tag *p = new Tag( iq, "ping" );
    p->addAttribute( "xmlns", XMLNS_XMPP_PING );

    send( iq );
  }

  const std::string ClientBase::getID()
  {
    std::ostringstream oss;
    oss << ++m_idCount;
    return std::string( "uid" ) + oss.str();
  }

  bool ClientBase::checkStreamVersion( const std::string& version )
  {
    if( version.empty() )
      return false;

    int major = 0;
    int minor = 0;
    int myMajor = XMPP_STREAM_VERSION_MAJOR;

    size_t dot = version.find( "." );
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

  LogSink& ClientBase::logInstance()
  {
    return m_logInstance;
  }

  void ClientBase::handleStreamError( Stanza *stanza )
  {
    Tag::TagList& c = stanza->children();
    Tag::TagList::const_iterator it = c.begin();
    for( ; it != c.end(); ++it )
    {
      if( (*it)->name() == "bad-format" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorBadFormat;
      else if( (*it)->name() == "bad-namespace-prefix" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorBadNamespacePrefix;
      else if( (*it)->name() == "conflict" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorConflict;
      else if( (*it)->name() == "connection-timeout" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorConnectionTimeout;
      else if( (*it)->name() == "host-gone" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorHostGone;
      else if( (*it)->name() == "host-unknown" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorHostUnknown;
      else if( (*it)->name() == "improper-addressing" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorImproperAddressing;
      else if( (*it)->name() == "internal-server-error" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorInternalServerError;
      else if( (*it)->name() == "invalid-from" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorInvalidFrom;
      else if( (*it)->name() == "invalid-id" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorInvalidId;
      else if( (*it)->name() == "invalid-namespace" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorInvalidNamespace;
      else if( (*it)->name() == "invalid-xml" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorInvalidXml;
      else if( (*it)->name() == "not-authorized" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorNotAuthorized;
      else if( (*it)->name() == "policy-violation" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorPolicyViolation;
      else if( (*it)->name() == "remote-connection-failed" &&
                 (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorRemoteConnectionFailed;
      else if( (*it)->name() == "resource-constraint" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorResourceConstraint;
      else if( (*it)->name() == "restricted-xml" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorRestrictedXml;
      else if( (*it)->name() == "see-other-host" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
      {
        m_streamError = StreamErrorSeeOtherHost;
        m_streamErrorCData = stanza->findChild( "see-other-host" )->cdata();
      }
      else if( (*it)->name() == "system-shutdown" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorSystemShutdown;
      else if( (*it)->name() == "undefined-condition" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorUndefinedCondition;
      else if( (*it)->name() == "unsupported-encoding" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorUnsupportedEncoding;
      else if( (*it)->name() == "unsupported-stanza-type" &&
                 (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorUnsupportedStanzaType;
      else if( (*it)->name() == "unsupported-version" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorUnsupportedVersion;
      else if( (*it)->name() == "xml-not-well-formed" && (*it)->hasAttribute( "xmlns", XMLNS_XMPP_STREAM ) )
        m_streamError = StreamErrorXmlNotWellFormed;
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
  }

  const std::string ClientBase::streamErrorText( const std::string& lang ) const
  {
    StringMap::const_iterator it = m_streamErrorText.find( lang );
    if( it != m_streamErrorText.end() )
      return (*it).second;
    else
      return "";
  }

  void ClientBase::registerMessageSessionHandler( MessageSessionHandler *msh, int types )
  {
    if( types & StanzaMessageChat || types == 0 )
      m_messageSessionHandlerChat = msh;

    if( types & StanzaMessageNormal || types == 0 )
      m_messageSessionHandlerNormal = msh;

    if( types & StanzaMessageGroupchat || types == 0 )
      m_messageSessionHandlerGroupchat = msh;

    if( types & StanzaMessageHeadline || types == 0 )
      m_messageSessionHandlerHeadline = msh;
  }

  int ClientBase::fileDescriptor()
  {
    if( m_connection )
    {
      m_fdRequested = true;
      return m_connection->fileDescriptor();
    }
    else
      return -1;
  }

  void ClientBase::registerPresenceHandler( PresenceHandler *ph )
  {
    if( ph )
      m_presenceHandlers.push_back( ph );
  }

  void ClientBase::removePresenceHandler( PresenceHandler *ph )
  {
    if( ph )
      m_presenceHandlers.remove( ph );
  }

  void ClientBase::registerIqHandler( IqHandler *ih, const std::string& xmlns )
  {
    if( ih && !xmlns.empty() )
      m_iqNSHandlers[xmlns] = ih;
  }

  void ClientBase::trackID( IqHandler *ih, const std::string& id, int context )
  {
    if( ih && !id.empty() )
    {
      TrackStruct track;
      track.ih = ih;
      track.context = context;
      m_iqIDHandlers[id] = track;
    }
  }

  void ClientBase::removeIqHandler( const std::string& xmlns )
  {
    if( !xmlns.empty() )
      m_iqNSHandlers.erase( xmlns );
  }

  void ClientBase::registerMessageHandler( const std::string& jid, MessageHandler *mh, int types )
  {
    if( mh && !jid.empty() )
    {
      JidHandlerStruct jhs;
      jhs.mh = mh;
      jhs.types = ( types )?( types ):( StanzaSubUndefined );
      m_messageJidHandlers[jid] = jhs;
    }
  }

  void ClientBase::registerMessageHandler( MessageHandler *mh )
  {
    if( mh )
      m_messageHandlers.push_back( mh );
  }

  void ClientBase::removeMessageHandler( const std::string& jid )
  {
    MessageJidHandlerMap::iterator it = m_messageJidHandlers.find( jid );
    if( it != m_messageJidHandlers.end() )
      m_messageJidHandlers.erase( it );
  }

  void ClientBase::removeMessageHandler( MessageHandler *mh )
  {
    if( mh )
      m_messageHandlers.remove( mh );
  }

  void ClientBase::registerSubscriptionHandler( SubscriptionHandler *sh )
  {
    if( sh )
      m_subscriptionHandlers.push_back( sh );
  }

  void ClientBase::registerTagHandler( TagHandler *th, const std::string& tag, const std::string& xmlns )
  {
    if( th && !tag.empty() )
    {
      TagHandlerStruct ths;
      ths.tag = tag;
      ths.xmlns = xmlns;
      ths.th = th;
      m_tagHandlers.push_back( ths );
    }
  }

  void ClientBase::registerStatisticsHandler( StatisticsHandler *sh )
  {
    if( sh )
      m_statisticsHandler = sh;
  }

  void ClientBase::registerMUCInvitationHandler( MUCInvitationHandler *mih )
  {
    if( mih )
    {
      m_mucInvitationHandler = mih;
      m_disco->addFeature( XMLNS_MUC );
    }
  }

  void ClientBase::removeStatisticsHandler()
  {
    m_statisticsHandler = 0;
  }

  void ClientBase::removeSubscriptionHandler( SubscriptionHandler *sh )
  {
    if( sh )
      m_subscriptionHandlers.remove( sh );
  }

  void ClientBase::registerConnectionListener( ConnectionListener *cl )
  {
    if( cl )
      m_connectionListeners.push_back( cl );
  }

  void ClientBase::removeConnectionListener( ConnectionListener *cl )
  {
    if( cl )
      m_connectionListeners.remove( cl );
  }

  void ClientBase::removeTagHandler( TagHandler *th, const std::string& tag, const std::string& xmlns )
  {
    if( th )
    {
      TagHandlerList::iterator it = m_tagHandlers.begin();
      for( ; it != m_tagHandlers.end(); ++it )
      {
        if( (*it).th == th && (*it).tag == tag && (*it).xmlns == xmlns )
          m_tagHandlers.erase( it );
      }
    }
  }

  void ClientBase::removeMUCInvitationHandler()
  {
    m_mucInvitationHandler = 0;
    m_disco->removeFeature( XMLNS_MUC );
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

    cleanup();
  }

  bool ClientBase::notifyOnTLSConnect( const CertInfo& info )
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( ; it != m_connectionListeners.end(); ++it )
    {
      if( !(*it)->onTLSConnect( info ) )
        return false;
    }

    m_stats.encryption = true;

    return true;
  }

  void ClientBase::notifyOnResourceBindError( ResourceBindError error )
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( ; it != m_connectionListeners.end(); ++it )
    {
      (*it)->onResourceBindError( error );
    }
  }

  void ClientBase::notifyOnSessionCreateError( SessionCreateError error )
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

    if( !res && ( stanza->type() == StanzaIq ) &&
         ( ( stanza->subtype() == StanzaIqGet ) || ( stanza->subtype() == StanzaIqSet ) ) )
    {
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "result" );
      iq->addAttribute( "id", stanza->id() );
      iq->addAttribute( "to", stanza->from().full() );
      send( iq );
    }
  }

  void ClientBase::notifyMessageHandlers( Stanza *stanza )
  {
    if( m_mucInvitationHandler )
    {
      Tag *x = stanza->findChild( "x", "xmlns", XMLNS_MUC_USER );
      if( x && x->hasChild( "invite" ) )
      {
        Tag *i = x->findChild( "invite" );
        JID invitee( i->findAttribute( "from" ) );
        std::string reason;
        std::string password;
        bool cont = false;
        if( i->hasChild( "reason" ) )
          reason = i->findChild( "reason" )->cdata();
        if( i->hasChild( "continue" ) )
          cont = true;
        if( x->hasChild( "password" ) )
          password = x->findChild( "password" )->cdata();

        m_mucInvitationHandler->handleMUCInvitation( stanza->from(), invitee, reason,
                                                     stanza->body(), password, cont );
        return;
      }
    }

    MessageJidHandlerMap::const_iterator it1 = m_messageJidHandlers.find( stanza->from().full() );
    if( it1 != m_messageJidHandlers.end()
        && ( (*it1).second.types & stanza->subtype() || (*it1).second.types == StanzaSubUndefined ) )
    {
      (*it1).second.mh->handleMessage( stanza );
      return;
    }

    it1 = m_messageJidHandlers.find( stanza->from().bare() );
    if( it1 != m_messageJidHandlers.end()
        && ( (*it1).second.types & stanza->subtype() || (*it1).second.types == StanzaSubUndefined ) )
    {
      (*it1).second.mh->handleMessage( stanza );
      return;
    }

    bool haveSessionHandler = false;
    MessageSession *session = new MessageSession( this, stanza->from(), true, stanza->subtype() );
    if( stanza->subtype() == StanzaMessageChat && m_messageSessionHandlerChat )
    {
      haveSessionHandler = true;
      m_messageSessionHandlerChat->handleMessageSession( session );
    }
    else if( stanza->subtype() == StanzaMessageNormal && m_messageSessionHandlerNormal )
    {
      haveSessionHandler = true;
      m_messageSessionHandlerNormal->handleMessageSession( session );
    }
    else if( stanza->subtype() == StanzaMessageGroupchat && m_messageSessionHandlerGroupchat )
    {
      haveSessionHandler = true;
      m_messageSessionHandlerGroupchat->handleMessageSession( session );
    }
    else if( stanza->subtype() == StanzaMessageHeadline && m_messageSessionHandlerHeadline )
    {
      haveSessionHandler = true;
      m_messageSessionHandlerHeadline->handleMessageSession( session );
    }

    if( haveSessionHandler )
    {
      notifyMessageHandlers( stanza );
      return;
    }
    else
      delete session;


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

  void ClientBase::cleanup()
  {

  }

}
