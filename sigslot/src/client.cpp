/*
  Copyright (c) 2004-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#include "config.h"

#include "client.h"
#include "capabilities.h"
#include "rostermanager.h"
#include "disco.h"
#include "error.h"
#include "logsink.h"
#include "nonsaslauth.h"
#include "prep.h"
#include "stanzaextensionfactory.h"
#include "stanzaextension.h"
#include "tag.h"
#include "connectiontls.h"
#include "connectioncompression.h"
#include "util.h"

#if !defined( _WIN32 ) && !defined( _WIN32_WCE )
# include <unistd.h>
#endif

namespace gloox
{

  // ---- Client::ResourceBind ----
  Client::ResourceBind::ResourceBind( const std::string& resource, bool bind )
    : StanzaExtension( ExtResourceBind ), m_jid( JID() ), m_bind( bind )
  {
    prep::resourceprep( resource, m_resource );
    m_valid = true;
  }

  Client::ResourceBind::ResourceBind( const Tag* tag )
    : StanzaExtension( ExtResourceBind ), m_resource( EmptyString ), m_bind( true )
  {
    if( !tag )
      return;

    if( tag->name() == "unbind" )
      m_bind = false;
    else if( tag->name() == "bind" )
      m_bind = true;
    else
      return;

    if( tag->hasChild( "jid" ) )
      m_jid.setJID( tag->findChild( "jid" )->cdata() );
    else if( tag->hasChild( "resource" ) )
      m_resource = tag->findChild( "resource" )->cdata();

    m_valid = true;
  }

  Client::ResourceBind::~ResourceBind()
  {
  }

  const std::string& Client::ResourceBind::filterString() const
  {
    static const std::string filter = "/iq/bind[@xmlns='" + XMLNS_STREAM_BIND + "']"
        "|/iq/unbind[@xmlns='" + XMLNS_STREAM_BIND + "']";
    return filter;
  }

  Tag* Client::ResourceBind::tag() const
  {
    if( !m_valid )
      return 0;

    Tag* t = new Tag( m_bind ? "bind" : "unbind" );
    t->setXmlns( XMLNS_STREAM_BIND );

    if( m_bind && m_resource.empty() && m_jid )
      new Tag( t, "jid", m_jid.full() );
    else
      new Tag( t, "resource", m_resource );

    return t;
  }
  // ---- ~Client::ResourceBind ----

  // ---- Client::SessionCreation ----
  Tag* Client::SessionCreation::tag() const
  {
    Tag* t = new Tag( "session" );
    t->setXmlns( XMLNS_STREAM_SESSION );
    return t;
  }
  // ---- Client::SessionCreation ----

  // ---- Client ----
  Client::Client( const std::string& server )
    : ClientBase( XMLNS_CLIENT, server ),
      m_rosterManager( 0 ), m_auth( 0 ),
      m_presence( Presence::Available, JID() ), m_resourceBound( false ),
      m_forceNonSasl( false ), m_manageRoster( true ),
      m_streamFeatures( 0 )
  {
    m_jid.setServer( server );
    init();
  }

  Client::Client( const JID& jid, const std::string& password, int port )
    : ClientBase( XMLNS_CLIENT, password, EmptyString, port ),
      m_rosterManager( 0 ), m_auth( 0 ),
      m_presence( Presence::Available, JID() ), m_resourceBound( false ),
      m_forceNonSasl( false ), m_manageRoster( true ),
      m_streamFeatures( 0 )
  {
    m_jid = jid;
    m_server = m_jid.serverRaw();
    init();
  }

  Client::~Client()
  {
    delete m_rosterManager;
    delete m_auth;
  }

  void Client::init()
  {
    m_rosterManager = new RosterManager( this );
    m_disco->setIdentity( "client", "bot" );
    registerStanzaExtension( new ResourceBind( 0 ) );
    registerStanzaExtension( new Capabilities() );
    m_presenceExtensions.push_back( new Capabilities( m_disco ) );
  }

  void Client::setUsername( const std::string &username )
  {
    m_jid.setUsername( username );
  }

  bool Client::handleNormalNode( Tag* tag )
  {
    if( tag->name() == "features" && tag->xmlns() == XMLNS_STREAM )
    {
      m_streamFeatures = getStreamFeatures( tag );

      if( !m_encryptionActive && m_tls == TLSRequired
          && ( !hasTls() || ( ( m_streamFeatures & StreamFeatureStartTls ) != StreamFeatureStartTls ) ) )
      {
        if( !hasTls() )
          logInstance().err( LogAreaClassClient, "Client is configured to require"
                                  " TLS but TLS support is not compiled into gloox." );
        else
          logInstance().err( LogAreaClassClient, "Client is configured to require"
                                  " TLS but the server didn't offer TLS." );
        disconnect( ConnTlsNotAvailable );
      }
      else if( m_tls > TLSDisabled && hasTls() && !m_encryptionActive
               && ( !m_encryption || m_encryption->state() == StateDisconnected )
               && ( ( m_streamFeatures & StreamFeatureStartTls ) == StreamFeatureStartTls ) )
      {
        onStreamEvent( StreamEventEncryption );
        startTls();
      }
      else if( hasCompression() && !m_compressionActive
               && ( !m_compression || m_compression->state() == StateDisconnected )
               && ( ( m_streamFeatures & StreamFeatureCompressZlib ) == StreamFeatureCompressZlib ) )
      {
        onStreamEvent( StreamEventCompression );
        logInstance().warn( LogAreaClassClient, "The server offers compression, but negotiating Compression at this stage is not recommended. See XEP-0170 for details. We'll continue anyway." );
        negotiateCompression( StreamFeatureCompressZlib );
      }
      else if( m_sasl )
      {
        if( m_authed )
        {
          if( m_streamFeatures & StreamFeatureBind )
          {
            onStreamEvent( StreamEventResourceBinding );
            bindResource( resource() );
          }
        }
        else if( !username().empty() && !password().empty() )
        {
          if( !login() )
          {
            logInstance().err( LogAreaClassClient, "The server doesn't support"
                                           " any auth mechanisms we know about" );
            disconnect( ConnNoSupportedAuth );
          }
        }
        else if( !m_clientCerts.empty() && !m_clientKey.empty()
                 && m_streamFeatures & SaslMechExternal && m_availableSaslMechs & SaslMechExternal )
        {
          onStreamEvent( StreamEventAuthentication );
          startSASL( SaslMechExternal );
        }
#if defined( _WIN32 ) && !defined( __SYMBIAN32__ )
        else if( m_streamFeatures & SaslMechGssapi && m_availableSaslMechs & SaslMechGssapi )
        {
          onStreamEvent( StreamEventAuthentication );
          startSASL( SaslMechGssapi );
        }
        else if( m_streamFeatures & SaslMechNTLM && m_availableSaslMechs & SaslMechNTLM )
        {
          onStreamEvent( StreamEventAuthentication );
          startSASL( SaslMechNTLM );
        }
#endif
        else if( m_streamFeatures & SaslMechAnonymous
                 && m_availableSaslMechs & SaslMechAnonymous )
        {
          onStreamEvent( StreamEventAuthentication );
          startSASL( SaslMechAnonymous );
        }
        else
        {
          onStreamEvent( StreamEventFinished );
          connected();
        }
      }
      else if( hasCompression()
               && ( !m_compression || m_compression->state() == StateDisconnected )
               && ( m_streamFeatures & StreamFeatureCompressZlib ) )
      {
        onStreamEvent( StreamEventCompression );
        negotiateCompression( StreamFeatureCompressZlib );
      }
//       else if( ( m_streamFeatures & StreamFeatureCompressDclz )
//               && m_connection->initCompression( StreamFeatureCompressDclz ) )
//       {
//         negotiateCompression( StreamFeatureCompressDclz );
//       }
      else if( m_streamFeatures & StreamFeatureIqAuth )
      {
        onStreamEvent( StreamEventAuthentication );
        nonSaslLogin();
      }
      else
      {
        logInstance().err( LogAreaClassClient, "fallback: the server doesn't "
                                   "support any auth mechanisms we know about" );
        disconnect( ConnNoSupportedAuth );
      }
    }
    else
    {
      const std::string& name  = tag->name();
      const std::string& xmlns = tag->xmlns();
      if( name == "proceed" && xmlns == XMLNS_STREAM_TLS && hasTls() )
      {
        logInstance().dbg( LogAreaClassClient, "Starting TLS handshake..." );

        m_connection->connected.Disconnect( this );
        m_connection->disconnected.Disconnect( this );
        m_connection->dataReceived.Disconnect( this );

        if( m_encryption )
          m_encryption->setConnectionImpl( m_connection );
        else
        {
          m_encryption = new ConnectionTLS( m_connection, m_logInstance );
        }
        m_encryption->setCACerts( m_cacerts );
        m_encryption->setClientCert( m_clientKey, m_clientCerts );
        m_connection = m_encryption;
        m_connection->connected.Connect( (ClientBase*)this, &ClientBase::handleConnect );
        m_connection->disconnected.Connect( (ClientBase*)this, &ClientBase::handleDisconnect );
        m_connection->dataReceived.Connect( (ClientBase*)this, &ClientBase::handleReceivedData );
        ((ConnectionTLS*)m_connection)->handshakeFinished.Connect( (ClientBase*)this, &ClientBase::handleHandshakeResult );
        m_connection->connect();
      }
      else if( name == "failure" )
      {
        if( xmlns == XMLNS_STREAM_TLS )
        {
          logInstance().err( LogAreaClassClient, "TLS handshake failed (server-side)!" );
          disconnect( ConnTlsFailed );
        }
        else if( xmlns == XMLNS_COMPRESSION )
        {
          logInstance().err( LogAreaClassClient, "Stream compression init failed!" );
          disconnect( ConnCompressionFailed );
        }
        else if( xmlns == XMLNS_STREAM_SASL )
        {
          logInstance().err( LogAreaClassClient, "SASL authentication failed!" );
          processSASLError( tag );
          disconnect( ConnAuthenticationFailed );
        }
      }
      else if( name == "compressed" && xmlns == XMLNS_COMPRESSION )
      {
        logInstance().dbg( LogAreaClassClient, "Stream compression initialized" );
        if( m_compression )
          m_compression->setConnectionImpl( m_connection );
        else
          m_compression = new ConnectionCompression( m_connection, m_logInstance );
        m_connection = m_compression;
        m_compression->connect();
        header();
      }
      else if( name == "challenge" && xmlns == XMLNS_STREAM_SASL )
      {
        logInstance().dbg( LogAreaClassClient, "Processing SASL challenge" );
        processSASLChallenge( tag->cdata() );
      }
      else if( name == "success" && xmlns == XMLNS_STREAM_SASL )
      {
        logInstance().dbg( LogAreaClassClient, "SASL authentication successful" );
        processSASLSuccess();
        setAuthed( true );
        header();
      }
      else
        return false;
    }

    return true;
  }

  int Client::getStreamFeatures( Tag* tag )
  {
    if( tag->name() != "features" || tag->xmlns() != XMLNS_STREAM )
      return 0;

    int features = 0;

    if( tag->hasChild( "starttls", XMLNS, XMLNS_STREAM_TLS ) )
      features |= StreamFeatureStartTls;

    if( tag->hasChild( "mechanisms", XMLNS, XMLNS_STREAM_SASL ) )
      features |= getSaslMechs( tag->findChild( "mechanisms" ) );

    if( tag->hasChild( "bind", XMLNS, XMLNS_STREAM_BIND ) )
      features |= StreamFeatureBind;

    if( tag->hasChild( "unbind", XMLNS, XMLNS_STREAM_BIND ) )
      features |= StreamFeatureUnbind;

    if( tag->hasChild( "session", XMLNS, XMLNS_STREAM_SESSION ) )
      features |= StreamFeatureSession;

    if( tag->hasChild( "auth", XMLNS, XMLNS_STREAM_IQAUTH ) )
      features |= StreamFeatureIqAuth;

    if( tag->hasChild( "register", XMLNS, XMLNS_STREAM_IQREGISTER ) )
      features |= StreamFeatureIqRegister;

    if( tag->hasChild( "compression", XMLNS, XMLNS_STREAM_COMPRESS ) )
      features |= getCompressionMethods( tag->findChild( "compression" ) );

    if( features == 0 )
      features = StreamFeatureIqAuth;

    return features;
  }

  int Client::getSaslMechs( Tag* tag )
  {
    int mechs = SaslMechNone;

    const std::string mech = "mechanism";

    if( tag->hasChildWithCData( mech, "DIGEST-MD5" ) )
      mechs |= SaslMechDigestMd5;

    if( tag->hasChildWithCData( mech, "PLAIN" ) )
      mechs |= SaslMechPlain;

    if( tag->hasChildWithCData( mech, "ANONYMOUS" ) )
      mechs |= SaslMechAnonymous;

    if( tag->hasChildWithCData( mech, "EXTERNAL" ) )
      mechs |= SaslMechExternal;

    if( tag->hasChildWithCData( mech, "GSSAPI" ) )
      mechs |= SaslMechGssapi;

    if( tag->hasChildWithCData( mech, "NTLM" ) )
      mechs |= SaslMechNTLM;

    return mechs;
  }

  int Client::getCompressionMethods( Tag* tag )
  {
    int meths = 0;

    if( tag->hasChildWithCData( "method", "zlib" ) )
      meths |= StreamFeatureCompressZlib;

    if( tag->hasChildWithCData( "method", "lzw" ) )
      meths |= StreamFeatureCompressDclz;

    return meths;
  }

  bool Client::login()
  {
    bool retval = true;

    if( m_streamFeatures & SaslMechDigestMd5 && m_availableSaslMechs & SaslMechDigestMd5
        && !m_forceNonSasl )
    {
      onStreamEvent( StreamEventAuthentication );
      startSASL( SaslMechDigestMd5 );
    }
    else if( m_streamFeatures & SaslMechPlain && m_availableSaslMechs & SaslMechPlain
             && !m_forceNonSasl )
    {
      onStreamEvent( StreamEventAuthentication );
      startSASL( SaslMechPlain );
    }
    else if( m_streamFeatures & StreamFeatureIqAuth || m_forceNonSasl )
    {
      onStreamEvent( StreamEventAuthentication );
      nonSaslLogin();
    }
    else
      retval = false;

    return retval;
  }

  void Client::handleIqIDForward( const IQ& iq, int context )
  {
    switch( context )
    {
      case CtxResourceUnbind:
        // we don't store known resources anyway
        break;
      case CtxResourceBind:
        processResourceBind( iq );
        break;
      case CtxSessionEstablishment:
        processCreateSession( iq );
        break;
      default:
        break;
    }
  }

  bool Client::bindOperation( const std::string& resource, bool bind )
  {
    if( !( m_streamFeatures & StreamFeatureUnbind ) && m_resourceBound )
      return false;

    IQ iq( IQ::Set, JID(), getID() );
    iq.addExtension( new ResourceBind( resource, bind ) );

    send( iq, this, bind ? CtxResourceBind : CtxResourceUnbind );
    return true;
  }

  bool Client::selectResource( const std::string& resource )
  {
    if( !( m_streamFeatures & StreamFeatureUnbind ) )
      return false;

    m_selectedResource = resource;

    return true;
  }

  void Client::processResourceBind( const IQ& iq )
  {
    switch( iq.subtype() )
    {
      case IQ::Result:
      {
        const ResourceBind* rb = iq.findExtension<ResourceBind>( ExtResourceBind );
        if( !rb || !rb->jid() )
        {
          onResourceBindError( 0 );
          break;
        }

        m_jid = rb->jid();
        m_resourceBound = true;
        m_selectedResource = m_jid.resource();
        onResourceBind( m_jid.resource() );

        if( m_streamFeatures & StreamFeatureSession )
          createSession();
        else
          connected();
        break;
      }
      case IQ::Error:
      {
        onResourceBindError( iq.error() );
        break;
      }
      default:
        break;
    }
  }

  void Client::createSession()
  {
    onStreamEvent( StreamEventSessionCreation );
    IQ iq( IQ::Set, JID(), getID() );
    iq.addExtension( new SessionCreation() );
    send( iq, this, CtxSessionEstablishment );
  }

  void Client::processCreateSession( const IQ& iq )
  {
    switch( iq.subtype() )
    {
      case IQ::Result:
        connected();
        break;
      case IQ::Error:
        onSessionCreateError( iq.error() );
        break;
      default:
        break;
    }
  }

  void Client::negotiateCompression( StreamFeature method )
  {
    Tag* t = new Tag( "compress", XMLNS, XMLNS_COMPRESSION );

    if( method == StreamFeatureCompressZlib )
      new Tag( t, "method", "zlib" );

    if( method == StreamFeatureCompressDclz )
      new Tag( t, "method", "lzw" );

    send( t );
  }

  void Client::setPresence( Presence::PresenceType pres, int priority,
                            const std::string& status )
  {
    m_presence.setPresence( pres );
    m_presence.setPriority( priority );
    m_presence.addStatus( status );
    sendPresence( m_presence );
  }

  void Client::setPresence( const JID& to, Presence::PresenceType pres, int priority,
                            const std::string& status )
  {
    Presence p( pres, to, status, priority );
    sendPresence( p );
  }

  void Client::sendPresence( Presence& pres )
  {
    if( state() < StateConnected )
      return;

    send( pres );
  }

  void Client::disableRoster()
  {
    m_manageRoster = false;
    delete m_rosterManager;
    m_rosterManager = 0;
  }

  void Client::nonSaslLogin()
  {
    if( !m_auth )
      m_auth = new NonSaslAuth( this );
    m_auth->doAuth( m_sid );
  }

  void Client::connected()
  {
    if( m_authed )
    {
      if( m_manageRoster )
      {
        onStreamEvent( StreamEventRoster );
        m_rosterManager->fill();
      }
      else
        rosterFilled();
    }
    else
    {
      onStreamEvent( StreamEventFinished );
      onConnect();
    }
  }

  void Client::rosterFilled()
  {
    sendPresence( m_presence );
    onStreamEvent( StreamEventFinished );
    onConnect();
  }

  void Client::disconnect()
  {
    disconnect( ConnUserDisconnected );
  }

  void Client::disconnect( ConnectionError reason )
  {
    m_resourceBound = false;
    m_authed = false;
    m_streamFeatures = 0;
    ClientBase::disconnect( reason );
  }

  void Client::cleanup()
  {
    m_authed = false;
    m_resourceBound = false;
    m_streamFeatures = 0;
  }

}
