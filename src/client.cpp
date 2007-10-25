/*
  Copyright (c) 2004-2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifdef _WIN32
# include "../config.h.win"
#elif defined( _WIN32_WCE )
# include "../config.h.win"
#else
# include "config.h"
#endif

#include "client.h"
#include "capabilities.h"
#include "rostermanager.h"
#include "disco.h"
#include "error.h"
#include "logsink.h"
#include "nonsaslauth.h"
#include "seresourcebind.h"
#include "stanzaextensionfactory.h"
#include "stanzaextension.h"
#include "tag.h"
#include "tlsbase.h"
#include "util.h"

#if !defined( _WIN32 ) && !defined( _WIN32_WCE )
# include <unistd.h>
#endif

#ifndef _WIN32_WCE
# include <iostream>
# include <sstream>
#else
# include <stdio.h>
#endif

namespace gloox
{

  Client::Client( const std::string& server )
    : ClientBase( XMLNS_CLIENT, server ),
      m_rosterManager( 0 ), m_auth( 0 ),
      m_presence( Presence::Available, JID() ), m_resourceBound( false ),
      m_forceNonSasl( false ), m_manageRoster( true ), m_doAuth( false ),
      m_streamFeatures( 0 ), m_priority( 0 )
  {
    m_jid.setServer( server );
    init();
  }

  Client::Client( const JID& jid, const std::string& password, int port )
    : ClientBase( XMLNS_CLIENT, password, EmptyString, port ),
      m_rosterManager( 0 ), m_auth( 0 ),
      m_presence( Presence::Available, JID() ), m_resourceBound( false ),
      m_forceNonSasl( false ), m_manageRoster( true ), m_doAuth( true ),
      m_streamFeatures( 0 ), m_priority( 0 )
  {
    m_jid = jid;
    m_server = m_jid.serverRaw();
    init();
  }

  Client::Client( const std::string& username, const std::string& password,
                  const std::string& server, const std::string& resource, int port )
    : ClientBase( XMLNS_CLIENT, password, server, port ),
      m_rosterManager( 0 ), m_auth( 0 ),
      m_presence( Presence::Available, JID() ), m_resourceBound( false ),
      m_forceNonSasl( false ), m_manageRoster( true ), m_doAuth( true ),
      m_streamFeatures( 0 ), m_priority( 0 )
  {
    m_jid.setUsername( username );
    m_jid.setServer( server );
    m_jid.setResource( resource );

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
    registerStanzaExtension( new SEResourceBind( 0 ) );
    registerStanzaExtension( new Capabilities( m_disco ) );
    m_presence.addExtension( new Capabilities( m_disco ) );
  }

  void Client::setUsername( const std::string &username )
  {
    m_jid.setUsername( username );
    m_doAuth = true;
  }

  bool Client::handleNormalNode( Tag* tag )
  {
    if( tag->name() == "features" && tag->xmlns() == XMLNS_STREAM )
    {
      m_streamFeatures = getStreamFeatures( tag );

      if( m_tls == TLSRequired
          && ( !m_encryption || !( m_streamFeatures & StreamFeatureStartTls ) ) )
      {
        logInstance().err( LogAreaClassClient, "Client is configured to require"
                                " TLS but either the server didn't offer TLS or"
                                " TLS support is not compiled in." );
        disconnect( ConnTlsNotAvailable );
      }
      else if( m_tls > TLSDisabled && m_encryption && !m_encryptionActive
          && ( m_streamFeatures & StreamFeatureStartTls ) )
      {
        notifyStreamEvent( StreamEventEncryption );
        startTls();
      }
      else if( m_sasl )
      {
        if( m_authed )
        {
          if( m_streamFeatures & StreamFeatureBind )
          {
            notifyStreamEvent( StreamEventResourceBinding );
            bindResource();
          }
        }
        else if( m_doAuth && !username().empty() && !password().empty() )
        {
          if( m_streamFeatures & SaslMechDigestMd5 && m_availableSaslMechs & SaslMechDigestMd5
              && !m_forceNonSasl )
          {
            notifyStreamEvent( StreamEventAuthentication );
            startSASL( SaslMechDigestMd5 );
          }
          else if( m_streamFeatures & SaslMechPlain && m_availableSaslMechs & SaslMechPlain
                   && !m_forceNonSasl )
          {
            notifyStreamEvent( StreamEventAuthentication );
            startSASL( SaslMechPlain );
          }
          else if( m_streamFeatures & StreamFeatureIqAuth || m_forceNonSasl )
          {
            notifyStreamEvent( StreamEventAuthentication );
            nonSaslLogin();
          }
          else
          {
            logInstance().err( LogAreaClassClient, "the server doesn't support"
                                           " any auth mechanisms we know about" );
            disconnect( ConnNoSupportedAuth );
          }
        }
        else if( m_doAuth && !m_clientCerts.empty() && !m_clientKey.empty()
                 && m_streamFeatures & SaslMechExternal && m_availableSaslMechs & SaslMechExternal )
        {
          notifyStreamEvent( StreamEventAuthentication );
          startSASL( SaslMechExternal );
        }
#ifdef _WIN32
        else if( m_doAuth && m_streamFeatures & SaslMechGssapi && m_availableSaslMechs & SaslMechGssapi )
        {
          notifyStreamEvent( StreamEventAuthentication );
          startSASL( SaslMechGssapi );
        }
#endif
        else if( m_doAuth && m_streamFeatures & SaslMechAnonymous
                 && m_availableSaslMechs & SaslMechAnonymous )
        {
          notifyStreamEvent( StreamEventAuthentication );
          startSASL( SaslMechAnonymous );
        }
        else
        {
          notifyStreamEvent( StreamEventFinished );
          connected();
        }
      }
      else if( m_compress && m_compression && !m_compressionActive
          && ( m_streamFeatures & StreamFeatureCompressZlib ) )
      {
        notifyStreamEvent( StreamEventCompression );
        negotiateCompression( StreamFeatureCompressZlib );
      }
//       else if( ( m_streamFeatures & StreamFeatureCompressDclz )
//               && m_connection->initCompression( StreamFeatureCompressDclz ) )
//       {
//         negotiateCompression( StreamFeatureCompressDclz );
//       }
      else if( m_streamFeatures & StreamFeatureIqAuth )
      {
        notifyStreamEvent( StreamEventAuthentication );
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
      const std::string& name  = tag->name(),
                         xmlns = tag->findAttribute( XMLNS );
      if( name == "proceed" && xmlns == XMLNS_STREAM_TLS )
      {
        logInstance().dbg( LogAreaClassClient, "starting TLS handshake..." );

        if( m_encryption )
        {
          m_encryptionActive = true;
          m_encryption->handshake();
        }
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
          logInstance().err( LogAreaClassClient, "stream compression init failed!" );
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
        logInstance().dbg( LogAreaClassClient, "stream compression inited" );
        m_compressionActive = true;
        header();
      }
      else if( name == "challenge" && xmlns == XMLNS_STREAM_SASL )
      {
        logInstance().dbg( LogAreaClassClient, "processing SASL challenge" );
        processSASLChallenge( tag->cdata() );
      }
      else if( name == "success" && xmlns == XMLNS_STREAM_SASL )
      {
        logInstance().dbg( LogAreaClassClient, "SASL authentication successful" );
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

  void Client::handleIqID( IQ* iq, int context )
  {
    switch( context )
    {
      case ResourceBind:
        processResourceBind( iq );
        break;
      case SessionEstablishment:
        processCreateSession( iq );
        break;
      default:
        break;
    }
  }

  void Client::bindResource()
  {
    if( !m_resourceBound )
    {
      IQ iq( IQ::Set, JID(), getID() );
      if( !resource().empty() )
        iq.addExtension( new SEResourceBind( resource() ) );

      send( iq, this, ResourceBind );
    }
  }

  void Client::processResourceBind( IQ* iq )
  {
    switch( iq->subtype() )
    {
      case IQ::Result:
      {
        const SEResourceBind* rb = static_cast<const SEResourceBind*>( iq->findExtension( ExtResourceBind ) );
        if( !rb || !rb->jid() )
        {
          notifyOnResourceBindError( RbErrorUnknownError );
          break;
        }

        m_jid = rb->jid();
        m_resourceBound = true;

        if( m_streamFeatures & StreamFeatureSession )
          createSession();
        else
          connected();
        break;
      }
      case IQ::Error:
      {
        const Error* error = iq->error();
        switch( error->type() )
        {
          case StanzaErrorTypeModify:
            if( error->error() == StanzaErrorBadRequest )
              notifyOnResourceBindError( RbErrorBadRequest );
            break;
          case StanzaErrorTypeCancel:
            if( error->error() == StanzaErrorNotAllowed )
              notifyOnResourceBindError( RbErrorNotAllowed );
            else if( error->error() == StanzaErrorConflict )
              notifyOnResourceBindError( RbErrorConflict );
            else
              notifyOnResourceBindError( RbErrorUnknownError );
            break;
          default:
            notifyOnResourceBindError( RbErrorUnknownError );
            break;
        }
        break;
      }
      default:
        break;
    }
  }

  void Client::createSession()
  {
    notifyStreamEvent( StreamEventSessionCreation );
    IQ iq( IQ::Set, JID(), getID(), XMLNS_STREAM_SESSION, "session" );
    send( iq, this, SessionEstablishment );
  }

  void Client::processCreateSession( IQ* iq )
  {
    switch( iq->subtype() )
    {
      case IQ::Result:
      {
        connected();
        break;
      }
      case IQ::Error:
      {
        const Error* error = iq->error();
        switch( error->type() )
        {
          case StanzaErrorTypeWait:
            if( error->error() == StanzaErrorInternalServerError )
              notifyOnSessionCreateError( ScErrorInternalServerError );
            break;
          case StanzaErrorTypeAuth:
            if( error->error() == StanzaErrorForbidden )
              notifyOnSessionCreateError( ScErrorForbidden );
            break;
          case StanzaErrorTypeCancel:
            if( error->error() == StanzaErrorConflict )
              notifyOnSessionCreateError( ScErrorConflict );
            break;
          default:
            notifyOnSessionCreateError( ScErrorUnknownError );
            break;
        }
        break;
      }
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

  void Client::setPresence( const JID& to, Presence::PresenceType pres, int priority,
                            const std::string& status )
  {
    Presence p( pres, to, status, priority );
    sendPresence( p );
  }

  void Client::sendPresence( const Presence& pres )
  {
    if( state() >= StateConnected )
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
        notifyStreamEvent( StreamEventRoster );
        m_rosterManager->fill();
      }
      else
        rosterFilled();
    }
    else
    {
      notifyStreamEvent( StreamEventFinished );
      notifyOnConnect();
    }
  }

  void Client::rosterFilled()
  {
    sendPresence( m_presence );
    notifyStreamEvent( StreamEventFinished );
    notifyOnConnect();
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
