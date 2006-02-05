/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "bareclient.h"

#include <string>
#include <sstream>

namespace gloox
{

  BareClient::BareClient( const JID& jid, const std::string& password, int port )
    : ClientBase( XMLNS_CLIENT, password, "", port ),
      m_autoPresence( false ), m_auth( 0 ), m_resourceBound( false ), m_forceNonSasl( false ),
      m_doAuth( false ), m_streamFeatures( 0 ), m_priority( -1 )
  {
    m_jid = jid;
    m_server = m_jid.serverRaw();
  }

  BareClient::BareClient( const std::string& ns, const std::string& password,
                          const std::string& server, int port )
    : ClientBase( ns, password, server, port ),
      m_autoPresence( false ), m_auth( 0 ), m_resourceBound( false ), m_forceNonSasl( false ),
      m_doAuth( false ), m_streamFeatures( 0 ), m_priority( -1 )
  {
  }

  BareClient::BareClient( const std::string& ns, const std::string& server, int port )
    : ClientBase( ns, server, port ),
      m_autoPresence( false ), m_auth( 0 ), m_resourceBound( false ), m_forceNonSasl( false ),
      m_doAuth( false ), m_streamFeatures( 0 ), m_priority( -1 )
  {
  }

  BareClient::~BareClient()
  {
    delete m_auth;
  }

  bool BareClient::handleNormalNode( Stanza *stanza )
  {
    if( stanza->name() == "stream:features" )
    {
      m_streamFeatures = getStreamFeatures( stanza );

#ifdef HAVE_TLS
      if( tls() && hasTls() && !m_connection->isSecure() && ( m_streamFeatures & STREAM_FEATURE_STARTTLS ) )
      {
        startTls();
        return true;
      }
#endif

#ifdef HAVE_ZLIB
      if( ( m_streamFeatures & STREAM_FEATURE_COMPRESS_ZLIB ) && m_connection->initCompression( true ) )
      {
        negotiateCompression( STREAM_FEATURE_COMPRESS_ZLIB );
        return true;
      }
//       if( ( m_streamFeatures & STREAM_FEATURE_COMPRESS_DCLZ ) && m_connection->initCompression( true ) )
//       {
//         negotiateCompression( STREAM_FEATURE_COMPRESS_DCLZ );
//         return true;
//       }
#endif

      if( sasl() )
      {
        if( m_authed )
        {
          if( m_streamFeatures & STREAM_FEATURE_BIND )
          {
            bindResource();
          }
        }
        else if( m_doAuth && !username().empty() && !password().empty() )
        {
          if( m_streamFeatures & STREAM_FEATURE_SASL_DIGESTMD5 && !m_forceNonSasl )
          {
            startSASL( SaslDigestMd5 );
          }
          else if( m_streamFeatures & STREAM_FEATURE_SASL_PLAIN && !m_forceNonSasl )
          {
            startSASL( SaslPlain );
          }
          else if( m_streamFeatures & STREAM_FEATURE_IQAUTH || m_forceNonSasl )
          {
            nonSaslLogin();
          }
          else
          {
            logInstance().log( LogLevelError, LogAreaClassClient,
            "the server doesn't support any auth mechanisms we know about" );
            disconnect( ConnNoSupportedAuth );
          }
        }
        else if( m_doAuth && !m_clientCerts.empty() && !m_clientKey.empty()
                  && m_streamFeatures & STREAM_FEATURE_SASL_EXTERNAL )
        {
          startSASL( SaslExternal );
        }
        else if( m_doAuth && m_streamFeatures & STREAM_FEATURE_SASL_ANONYMOUS )
        {
          startSASL( SaslAnonymous );
        }
        else
        {
          connected();
        }
      }
      else if( m_streamFeatures & STREAM_FEATURE_IQAUTH )
      {
        nonSaslLogin();
      }
      else
      {
        logInstance().log( LogLevelError, LogAreaClassClient,
        "the server doesn't support any auth mechanisms we know about" );
        disconnect( ConnNoSupportedAuth );
      }
    }
#ifdef HAVE_TLS
    else if( ( stanza->name() == "proceed" ) && stanza->hasAttribute( "xmlns", XMLNS_STREAM_TLS ) )
    {
      logInstance().log( LogLevelDebug, LogAreaClassClient, "starting TLS handshake..." );

      if( m_connection->tlsHandshake() )
      {
        if( !notifyOnTLSConnect( m_connection->fetchTLSInfo() ) )
          disconnect( ConnTlsFailed );
        else
        {
          std::ostringstream oss;
          if( m_connection->isSecure() )
          {
            oss << "connection encryption active";
            logInstance().log( LogLevelDebug, LogAreaClassClient, oss.str() );
          }
          else
          {
            oss << "connection not encrypted!";
            logInstance().log( LogLevelWarning, LogAreaClassClient, oss.str() );
          }

          header();
        }
      }
      else
        disconnect( ConnTlsFailed );
    }
      else if( ( stanza->name() == "failure" ) && stanza->hasAttribute( "xmlns", XMLNS_STREAM_TLS ) )
    {
      logInstance().log( LogLevelError, LogAreaClassClient, "TLS handshake failed!" );
      disconnect( ConnTlsFailed );
    }
#endif
#ifdef HAVE_ZLIB
    else if( ( stanza->name() == "failure" ) && stanza->hasAttribute( "xmlns", XMLNS_COMPRESSION ) )
    {
      logInstance().log( LogLevelError, LogAreaClassClient, "stream compression init failed!" );
      disconnect( ConnTlsFailed );
    }
    else if( ( stanza->name() == "compressed" ) && stanza->hasAttribute( "xmlns", XMLNS_COMPRESSION ) )
    {
      logInstance().log( LogLevelDebug, LogAreaClassClient, "stream compression inited" );
      m_connection->setCompression( true );
      header();
    }
#endif
    else if( ( stanza->name() == "challenge" ) && stanza->hasAttribute( "xmlns", XMLNS_STREAM_SASL ) )
    {
      logInstance().log( LogLevelDebug, LogAreaClassClient, "processing sasl challenge" );
      processSASLChallenge( stanza->cdata() );
    }
    else if( ( stanza->name() == "failure" ) && stanza->hasAttribute( "xmlns", XMLNS_STREAM_SASL ) )
    {
      logInstance().log( LogLevelError, LogAreaClassClient, "sasl authentication failed!" );
      processSASLError( stanza );
      disconnect( ConnAuthenticationFailed );
    }
    else if( ( stanza->name() == "success" ) && stanza->hasAttribute( "xmlns", XMLNS_STREAM_SASL ) )
    {
      logInstance().log( LogLevelDebug, LogAreaClassClient, "sasl auth successful" );
      setAuthed( true );
      header();
    }
    else
    {
      if( ( stanza->name() == "iq" ) && stanza->hasAttribute( "id", "bind" ) )
      {
        processResourceBind( stanza );
      }
      else if( ( stanza->name() == "iq" ) && stanza->hasAttribute( "id", "session" ) )
      {
        processCreateSession( stanza );
      }
      else
        return false;
    }

    return true;
  }

  int BareClient::getStreamFeatures( Stanza *stanza )
  {
    if( stanza->name() != "stream:features" )
      return 0;

    int features = 0;

    if( stanza->hasChild( "starttls", "xmlns", XMLNS_STREAM_TLS ) )
      features |= STREAM_FEATURE_STARTTLS;

    if( stanza->hasChild( "mechanisms", "xmlns", XMLNS_STREAM_SASL ) )
      features |= getSaslMechs( stanza->findChild( "mechanisms" ) );

    if( stanza->hasChild( "bind", "xmlns", XMLNS_STREAM_BIND ) )
      features |= STREAM_FEATURE_BIND;

    if( stanza->hasChild( "session", "xmlns", XMLNS_STREAM_SESSION ) )
      features |= STREAM_FEATURE_SESSION;

    if( stanza->hasChild( "auth", "xmlns", XMLNS_STREAM_IQAUTH ) )
      features |= STREAM_FEATURE_IQAUTH;

    if( stanza->hasChild( "register", "xmlns", XMLNS_STREAM_IQREGISTER ) )
      features |= STREAM_FEATURE_IQREGISTER;

    if( stanza->hasChild( "ack", "xmlns", XMLNS_STREAM_ACK ) )
      features |= STREAM_FEATURE_ACK;

    if( stanza->hasChild( "compression", "xmlns", XMLNS_STREAM_COMPRESS ) )
      features |= getCompressionMethods( stanza->findChild( "compression" ) );

    if( features == 0 )
      features = STREAM_FEATURE_IQAUTH;

    return features;
  }

  int BareClient::getSaslMechs( Tag *tag )
  {
    int mechs = 0;

    if( tag->hasChildWithCData( "mechanism", "DIGEST-MD5" ) )
      mechs |= STREAM_FEATURE_SASL_DIGESTMD5;

    if( tag->hasChildWithCData( "mechanism", "PLAIN" ) )
      mechs |= STREAM_FEATURE_SASL_PLAIN;

    if( tag->hasChildWithCData( "mechanism", "ANONYMOUS" ) )
      mechs |= STREAM_FEATURE_SASL_ANONYMOUS;

    if( tag->hasChildWithCData( "mechanism", "EXTERNAL" ) )
      mechs |= STREAM_FEATURE_SASL_EXTERNAL;

    return mechs;
  }

  int BareClient::getCompressionMethods( Tag *tag )
  {
    int meths = 0;

    if( tag->hasChildWithCData( "method", "zlib" ) )
      meths |= STREAM_FEATURE_COMPRESS_ZLIB;

    if( tag->hasChildWithCData( "method", "lzw" ) )
      meths |= STREAM_FEATURE_COMPRESS_DCLZ;

    return meths;
  }

  void BareClient::bindResource()
  {
    if( !m_resourceBound )
    {
      Tag *iq = new Tag( "iq" );
      iq->addAttribute( "type", "set" );
      iq->addAttribute( "id", "bind" );
      Tag *b = new Tag( iq, "bind" );
      b->addAttribute( "xmlns", XMLNS_STREAM_BIND );
      if( !resource().empty() )
        new Tag( b, "resource", resource() );

      send( iq );
    }
  }

  void BareClient::processResourceBind( Stanza *stanza )
  {
    switch( stanza->subtype() )
    {
      case StanzaIqResult:
      {
        Tag *bind = stanza->findChild( "bind" );
        Tag *jid = bind->findChild( "jid" );
        m_jid.setJID( jid->cdata() );
        m_resourceBound = true;

        if( m_streamFeatures & STREAM_FEATURE_SESSION )
          createSession();
        else
          connected();
        break;
      }
      case StanzaIqError:
      {
        Tag *error = stanza->findChild( "error" );
        if( stanza->hasChild( "error", "type", "modify" )
            && error->hasChild( "bad-request", "xmlns", XMLNS_XMPP_STANZAS ) )
        {
          notifyOnResourceBindError( RbErrorBadRequest );
        }
        else if( stanza->hasChild( "error", "type", "cancel" ) )
        {
          if( error->hasChild( "not-allowed", "xmlns", XMLNS_XMPP_STANZAS ) )
            notifyOnResourceBindError( RbErrorNotAllowed );
          else if( error->hasChild( "conflict", "xmlns", XMLNS_XMPP_STANZAS ) )
            notifyOnResourceBindError( RbErrorConflict );
          else
            notifyOnResourceBindError( RbErrorUnknownError );
        }
        else
          notifyOnResourceBindError( RbErrorUnknownError );
        break;
      }
      default:
        break;
    }
  }

  void BareClient::createSession()
  {
    Tag *iq = new Tag( "iq" );
    iq->addAttribute( "type", "set" );
    iq->addAttribute( "id", "session" );
    Tag *s = new Tag( iq, "session" );
    s->addAttribute( "xmlns", XMLNS_STREAM_SESSION );

    send( iq );
  }

  void BareClient::processCreateSession( Stanza *stanza )
  {
    switch( stanza->subtype() )
    {
      case StanzaIqResult:
      {
        connected();
        break;
      }
      case StanzaIqError:
      {
        Tag *error = stanza->findChild( "error" );
        if( stanza->hasChild( "error", "type", "wait" )
            && error->hasChild( "internal-server-error", "xmlns", XMLNS_XMPP_STANZAS ) )
        {
          notifyOnSessionCreateError( ScErrorInternalServerError );
        }
        else if( stanza->hasChild( "error", "type", "auth" )
                  && error->hasChild( "forbidden", "xmlns", XMLNS_XMPP_STANZAS ) )
        {
          notifyOnSessionCreateError( ScErrorForbidden );
        }
        else if( stanza->hasChild( "error", "type", "cancel" )
                  && error->hasChild( "conflict", "xmlns", XMLNS_XMPP_STANZAS ) )
        {
          notifyOnSessionCreateError( ScErrorConflict );
        }
        else
          notifyOnSessionCreateError( ScErrorUnknownError );
        break;
      }
      default:
        break;
    }
  }

  void BareClient::negotiateCompression( StreamFeaturesEnum method )
  {
    Tag *t = new Tag( "compress" );
    t->addAttribute( "xmlns", XMLNS_COMPRESSION );

    if( method == STREAM_FEATURE_COMPRESS_ZLIB )
      new Tag( t, "method", "zlib" );

    if( method == STREAM_FEATURE_COMPRESS_DCLZ )
      new Tag( t, "method", "lzw" );

    send( t );
  }

  void BareClient::connected()
  {
    if( m_authed )
    {
      if( m_autoPresence )
        sendInitialPresence();
    }

    notifyOnConnect();
  }

  void BareClient::nonSaslLogin()
  {
    m_auth = new NonSaslAuth( this, m_sid );
    m_auth->doAuth();
  }

  void BareClient::sendInitialPresence()
  {
    Tag *p = new Tag( "presence" );
    std::ostringstream oss;
    oss << m_priority;
    new Tag( p, "priority", oss.str() );

    send( p );
  }

  void BareClient::setInitialPriority( int priority )
  {
    if( priority < -128 )
      priority = -128;
    if( priority > 127 )
      priority = 127;

    m_priority = priority;
  }

}
